#pragma once

#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <string>
#include <string>

namespace edcolony {

// Skeleton for async HTTP client (implementation later)
class HttpClient {
public:
    HttpClient(asio::io_context& io, std::string base_uri, std::string user_agent);

    // GET /api/cmdr/{cmdr}/active
    asio::awaitable<std::string> getCmdrActive(const std::string& cmdr);
    // GET /api/cmdr/{cmdr}/primary
    asio::awaitable<std::string> getPrimary(const std::string& cmdr);
    // GET /api/cmdr/{cmdr}/fc/all
    asio::awaitable<std::string> getAllCmdrFCs(const std::string& cmdr);

private:
    asio::io_context& io_context_;
    std::string base_uri_;
    std::string user_agent_;
    std::string host_;
    std::string port_;
    std::string api_prefix_ {"/api"};
    std::string etag_;
    asio::awaitable<std::string> httpGet(const std::string& path, int timeout_ms = 5000, int retries = 2);
};

}


