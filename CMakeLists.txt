cmake_minimum_required(VERSION 3.24)

project(warframe_analytics
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Type de build" FORCE)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)

# ---------------------------------------------------------------------------
# libcurl
# ---------------------------------------------------------------------------
set(BUILD_CURL_EXE      OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING       OFF CACHE BOOL "" FORCE)
set(BUILD_LIBCURL_DOCS  OFF CACHE BOOL "" FORCE)
set(BUILD_MISC_DOCS     OFF CACHE BOOL "" FORCE)
set(ENABLE_CURL_MANUAL  OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP   ON  CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL     OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBSSH2    OFF CACHE BOOL "" FORCE)
set(USE_LIBIDN2         OFF CACHE BOOL "" FORCE)
set(USE_NGHTTP2         OFF CACHE BOOL "" FORCE)
set(CURL_BROTLI         OFF CACHE BOOL "" FORCE)
set(CURL_ZSTD           OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(CURL_USE_SCHANNEL ON  CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL  OFF CACHE BOOL "" FORCE)
else()
    set(CURL_USE_OPENSSL  ON  CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    curl
    URL https://github.com/curl/curl/releases/download/curl-8_11_1/curl-8.11.1.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS NAMES CURL
)

# ---------------------------------------------------------------------------
# nlohmann/json
# ---------------------------------------------------------------------------
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS NAMES nlohmann_json
)

FetchContent_MakeAvailable(curl nlohmann_json)


file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
