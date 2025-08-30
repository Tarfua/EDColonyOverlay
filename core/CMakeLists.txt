add_library(edcolony_core
    src/dummy.cpp
    src/journal.cpp
    src/http_client.cpp
    src/storage.cpp
    src/handlers.cpp
    src/net_sync.cpp
)

target_include_directories(edcolony_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(edcolony_core
    PUBLIC
        nlohmann_json::nlohmann_json
        Threads::Threads
        SQLite::SQLite3
)

target_compile_features(edcolony_core PUBLIC cxx_std_20)


