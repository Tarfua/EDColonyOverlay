add_library(edcolony_core
    src/dummy.cpp
)

target_include_directories(edcolony_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(edcolony_core
    PUBLIC
        nlohmann_json::nlohmann_json
)

target_compile_features(edcolony_core PUBLIC cxx_std_20)


