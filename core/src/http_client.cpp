#include "edcolony/http_client.hpp"

#include <asio/ip/tcp.hpp>
#include <asio/connect.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/redirect_error.hpp>
#include <asio/streambuf.hpp>
#include <asio/write.hpp>
#include <asio/read_until.hpp>
#include <charconv>
#include <random>
#include <asio/steady_timer.hpp>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::ip::tcp;
using asio::use_awaitable;

namespace edcolony {

static void parseBaseUri(const std::string& base, std::string& host, std::string& port) {
    // Very small parser for http://host[:port]
    std::string s = base;
    const std::string http = "http://";
    if (s.rfind(http, 0) == 0) s = s.substr(http.size());
    auto colon = s.find(':');
    if (colon == std::string::npos) {
        host = s;
        port = "80";
    } else {
        host = s.substr(0, colon);
        port = s.substr(colon + 1);
    }
}

HttpClient::HttpClient(asio::io_context& io, std::string base_uri, std::string user_agent)
    : io_context_(io), base_uri_(std::move(base_uri)), user_agent_(std::move(user_agent)) {
    parseBaseUri(base_uri_, host_, port_);
}

awaitable<std::string> HttpClient::httpGet(const std::string& path, int timeout_ms, int retries) {
    std::minstd_rand rng{static_cast<unsigned int>(std::random_device{}())};
    std::uniform_int_distribution<int> jitter(50, 200);
    for (int attempt = 0; attempt <= retries; ++attempt) {
        try {
            tcp::resolver resolver(io_context_);
            tcp::socket socket(io_context_);
            auto endpoints = co_await resolver.async_resolve(host_, port_, use_awaitable);
            co_await asio::async_connect(socket, endpoints, use_awaitable);

            std::string req;
            req += "GET " + path + " HTTP/1.1\r\n";
            req += "Host: " + host_ + "\r\n";
            req += "User-Agent: " + user_agent_ + "\r\n";
            req += "Accept: application/json\r\n";
            if (!etag_.empty()) req += "If-None-Match: " + etag_ + "\r\n";
            req += "Connection: close\r\n\r\n";
            co_await asio::async_write(socket, asio::buffer(req), use_awaitable);

            asio::streambuf response;
            co_await asio::async_read_until(socket, response, "\r\n\r\n", use_awaitable);
            std::istream rs(&response);
            std::string status_line;
            std::getline(rs, status_line);

            // parse headers for ETag
            std::string header;
            std::string etag_value;
            while (std::getline(rs, header) && header != "\r") {
                if (header.rfind("ETag:", 0) == 0) {
                    auto pos = header.find(':');
                    if (pos != std::string::npos) {
                        etag_value = header.substr(pos + 1);
                        if (!etag_value.empty() && etag_value.back() == '\r') etag_value.pop_back();
                        // trim leading spaces
                        while (!etag_value.empty() && (etag_value.front() == ' ' || etag_value.front() == '\t')) etag_value.erase(etag_value.begin());
                    }
                }
            }

            if (status_line.rfind("HTTP/1.1 304", 0) == 0 || status_line.rfind("HTTP/1.0 304", 0) == 0) {
                co_return std::string{}; // cache hit; caller can interpret empty as unchanged
            }
            if (status_line.rfind("HTTP/1.1 200", 0) != 0 && status_line.rfind("HTTP/1.0 200", 0) != 0) {
                throw std::runtime_error("non-200");
            }
            if (!etag_value.empty()) etag_ = etag_value;

            // Read remaining body
            std::string body;
            if (response.size() > 0) {
                body.assign(std::istreambuf_iterator<char>(rs), {});
            }
            asio::error_code ec;
            for (;;) {
                std::array<char, 4096> buf{};
                std::size_t n = socket.read_some(asio::buffer(buf), ec);
                if (n > 0) body.append(buf.data(), n);
                if (ec) break;
            }
            co_return body;
        } catch (...) {
            if (attempt == retries) break;
            asio::steady_timer t(io_context_);
            t.expires_after(std::chrono::milliseconds(jitter(rng)));
            co_await t.async_wait(use_awaitable);
        }
    }
    co_return std::string{};
}

awaitable<std::string> HttpClient::getCmdrActive(const std::string& cmdr) {
    co_return co_await httpGet(api_prefix_ + "/cmdr/" + cmdr + "/active");
}

awaitable<std::string> HttpClient::getPrimary(const std::string& cmdr) {
    co_return co_await httpGet(api_prefix_ + "/cmdr/" + cmdr + "/primary");
}

awaitable<std::string> HttpClient::getAllCmdrFCs(const std::string& cmdr) {
    co_return co_await httpGet(api_prefix_ + "/cmdr/" + cmdr + "/fc/all");
}

}


