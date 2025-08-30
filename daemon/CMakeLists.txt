add_executable(edcolony_daemon
    src/main.cpp
)

target_link_libraries(edcolony_daemon PRIVATE edcolony_core)
target_compile_features(edcolony_daemon PRIVATE cxx_std_20)

install(TARGETS edcolony_daemon RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


