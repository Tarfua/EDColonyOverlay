#pragma once

#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <string>

namespace edcolony {

// Skeleton for async HTTP client (implementation later)
class HttpClient {
public:
    explicit HttpClient(asio::io_context& io, std::string user_agent)
        : io_context_(io), user_agent_(std::move(user_agent)) {}

    // GET /api/cmdr/{cmdr}/active
    asio::awaitable<std::string> getCmdrActive(const std::string& cmdr) {
        co_return std::string{}; // stub
    }
    // GET /api/cmdr/{cmdr}/primary
    asio::awaitable<std::string> getPrimary(const std::string& cmdr) {
        co_return std::string{}; // stub
    }
    // GET /api/cmdr/{cmdr}/fc/all
    asio::awaitable<std::string> getAllCmdrFCs(const std::string& cmdr) {
        co_return std::string{}; // stub
    }

private:
    asio::io_context& io_context_;
    std::string user_agent_;
};

}


