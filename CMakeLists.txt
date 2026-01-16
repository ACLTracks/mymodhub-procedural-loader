cmake_minimum_required(VERSION 3.26)

project(mymodhub_procedural_loader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PLUGIN_NAME "mymodhub-procedural-loader")

# ----------------------------
# Dependencies
# ----------------------------

include(FetchContent)

# nlohmann_json: disable tests
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

# CommonLibSSE-NG respects this
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

# ----------------------------
# 1. fmt
# ----------------------------
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(fmt)

# ----------------------------
# 2. spdlog (Target-only)
# ----------------------------
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# ----------------------------
# 3. Spdlog Config Workaround
# ----------------------------
set(_LOCAL_PKG_DIR "${CMAKE_BINARY_DIR}/_local_pkgs/spdlog")
file(MAKE_DIRECTORY "${_LOCAL_PKG_DIR}")

file(WRITE "${_LOCAL_PKG_DIR}/spdlogConfig.cmake" [=[
if(NOT TARGET spdlog::spdlog)
  if(TARGET spdlog)
    add_library(spdlog::spdlog ALIAS spdlog)
  endif()
endif()
if(NOT TARGET spdlog::spdlog_header_only)
  if(TARGET spdlog_header_only)
    add_library(spdlog::spdlog_header_only ALIAS spdlog_header_only)
  endif()
endif()
]=])

set(spdlog_DIR "${_LOCAL_PKG_DIR}" CACHE PATH "" FORCE)

# ----------------------------
# 4. rapidcsv
# ----------------------------
FetchContent_Declare(
  rapidcsv
  GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
  GIT_TAG v8.84
)
FetchContent_MakeAvailable(rapidcsv)
set(RAPIDCSV_INCLUDE_DIRS "${rapidcsv_SOURCE_DIR}/src" CACHE PATH "" FORCE)

# ----------------------------
# 5. CommonLibSSE-NG
# ----------------------------

# Prevent install/export errors
set(CMAKE_SKIP_INSTALL_RULES ON)

set(COMMONLIBSSE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE-NG")

if (EXISTS "${COMMONLIBSSE_SOURCE_DIR}/CMakeLists.txt")
  message(STATUS "Using CommonLibSSE-NG submodule at: ${COMMONLIBSSE_SOURCE_DIR}")
  add_subdirectory("${COMMONLIBSSE_SOURCE_DIR}")
else()
  FetchContent_Declare(
    CommonLibSSE_NG
    GIT_REPOSITORY https://github.com/CharmedBaryon/CommonLibSSE-NG.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(CommonLibSSE_NG)

  # Make sure the helper include below points at the fetched source
  set(COMMONLIBSSE_SOURCE_DIR "${CommonLibSSE_NG_SOURCE_DIR}")
  message(STATUS "Using CommonLibSSE-NG via FetchContent at: ${COMMONLIBSSE_SOURCE_DIR}")
endif()

# This defines add_commonlibsse_plugin()
if (EXISTS "${COMMONLIBSSE_SOURCE_DIR}/cmake/CommonLibSSE.cmake")
  include("${COMMONLIBSSE_SOURCE_DIR}/cmake/CommonLibSSE.cmake")
else()
  message(FATAL_ERROR "CommonLibSSE.cmake not found at: ${COMMONLIBSSE_SOURCE_DIR}/cmake/CommonLibSSE.cmake")
endif()

# ----------------------------
# 6. nlohmann_json
# ----------------------------
set(NLOHMANN_JSON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann_json")
if (EXISTS "${NLOHMANN_JSON_SOURCE_DIR}/CMakeLists.txt")
  add_subdirectory("${NLOHMANN_JSON_SOURCE_DIR}")
else()
  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

# ----------------------------
# Plugin Target
# ----------------------------

set(MMH_SOURCES
  src/main.cpp
  src/Plugin.cpp
  src/Plugin.h
  src/Log.cpp
  src/Log.h
  src/Version.h
  src/mmh/PackPaths.h
  src/mmh/PackPaths.cpp
  src/mmh/PackLoader.h
  src/mmh/PackLoader.cpp
  src/mmh/Validate.h
  src/mmh/Validate.cpp
  src/mmh/Apply.h
  src/mmh/Apply.cpp
  src/racemenu/RaceMenuBridge.h
  src/racemenu/RaceMenuBridge.cpp
  src/mymodhub/packs/JsonUtil.h
  src/mymodhub/packs/JsonUtil.cpp
  src/mymodhub/packs/PackRegistry.h
  src/mymodhub/packs/PackRegistry.cpp
  src/mymodhub/packs/PackModels.h
)

add_commonlibsse_plugin(${PLUGIN_NAME}
  SOURCES ${MMH_SOURCES}
)

# Your PCH fix, kept as-is
target_precompile_headers(${PLUGIN_NAME} PUBLIC
  <cstdint>
  <array>
  <vector>
  <string>
  <string_view>
  <algorithm>
  <stdexcept>
)

target_include_directories(${PLUGIN_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${RAPIDCSV_INCLUDE_DIRS}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  nlohmann_json::nlohmann_json
)

if (MSVC)
  target_compile_options(${PLUGIN_NAME} PRIVATE /W4 /permissive- /EHsc)
endif()
