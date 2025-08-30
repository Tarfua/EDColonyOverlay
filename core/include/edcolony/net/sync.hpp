#pragma once

#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include "edcolony/http_client.hpp"
#include "edcolony/storage.hpp"
#include "edcolony/state.hpp"

namespace edcolony {

// Fetch primary buildId, active projects and fleet carriers for cmdr; update storage and state
asio::awaitable<void> syncCmdrSnapshot(HttpClient& client,
                                       Storage& storage,
                                       ColonyState& state,
                                       const std::string& cmdr);

}


