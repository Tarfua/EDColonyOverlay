cmake_minimum_required(VERSION 3.22)

project(EDColony VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)

# Dependencies via FetchContent
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# asio (standalone)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)

# spdlog (header-only for now)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(core)
add_subdirectory(daemon)


# Install systemd user unit (packaging)
install(FILES ${CMAKE_SOURCE_DIR}/packaging/systemd/edcolony-daemon.service
        DESTINATION lib/systemd/user)

# CPack packaging (TGZ and DEB)
set(CPACK_PACKAGE_NAME "edcolony-daemon")
set(CPACK_PACKAGE_VENDOR "EDColony")
set(CPACK_PACKAGE_CONTACT "EDColony Maintainers")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EDColony daemon for Elite Dangerous colonisation data")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "EDColony Maintainers")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libsqlite3-0")
set(CPACK_GENERATOR "TGZ;DEB")
include(CPack)

