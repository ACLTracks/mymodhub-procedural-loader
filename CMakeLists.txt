cmake_minimum_required(VERSION 3.26)

project(mymodhub_procedural_loader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PLUGIN_NAME "mymodhub-procedural-loader")

# ----------------------------
# Dependencies
# ----------------------------

include(FetchContent)

# nlohmann_json: disable tests globally (covers both add_subdirectory and FetchContent)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

# CommonLibSSE-NG respects this for its own tests in many setups
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

# ----------------------------
# fmt (target-only is fine)
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
# spdlog (target-only)
# ----------------------------
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)

# Make spdlog use external fmt (the one we just fetched)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)

# Keep spdlog lean
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spdlog)

# ----------------------------
# Provide a local spdlog "package config" for CommonLibSSE-NG
# (avoids broken build-tree export targets include issue)
# ----------------------------
set(_LOCAL_PKG_DIR "${CMAKE_BINARY_DIR}/_local_pkgs/spdlog")
file(MAKE_DIRECTORY "${_LOCAL_PKG_DIR}")

file(WRITE "${_LOCAL_PKG_DIR}/spdlogConfig.cmake" [=[
# Minimal config to satisfy find_package(spdlog CONFIG)
# We already built spdlog via FetchContent, so targets should exist.

if(NOT TARGET spdlog::spdlog)
  if(TARGET spdlog)
    add_library(spdlog::spdlog ALIAS spdlog)
  endif()
endif()

if(NOT TARGET spdlog::spdlog)
  message(FATAL_ERROR "spdlog target not found. FetchContent_MakeAvailable(spdlog) did not produce expected targets.")
endif()

# Some projects look for header-only target
if(NOT TARGET spdlog::spdlog_header_only)
  if(TARGET spdlog_header_only)
    add_library(spdlog::spdlog_header_only ALIAS spdlog_header_only)
  endif()
endif()
]=])

# Force CommonLibSSE-NG to use our local config
set(spdlog_DIR "${_LOCAL_PKG_DIR}" CACHE PATH "" FORCE)

# ----------------------------
# rapidcsv (CommonLibSSE-NG expects RAPIDCSV_INCLUDE_DIRS)
# ----------------------------
FetchContent_Declare(
  rapidcsv
  GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
  GIT_TAG v8.84
)
FetchContent_MakeAvailable(rapidcsv)

# rapidcsv is header-only; CommonLibSSE-NG looks for this var
set(RAPIDCSV_INCLUDE_DIRS "${rapidcsv_SOURCE_DIR}/src" CACHE PATH "" FORCE)

# ----------------------------
# CommonLibSSE-NG: prefer submodule, fallback to FetchContent
# ----------------------------
set(COMMONLIBSSE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE-NG")

# We are building a plugin, not installing CommonLibSSE, so skip install/export rules
set(CMAKE_SKIP_INSTALL_RULES ON)

if (EXISTS "${COMMONLIBSSE_SOURCE_DIR}/CMakeLists.txt")
  add_subdirectory("${COMMONLIBSSE_SOURCE_DIR}")
else()
  FetchContent_Declare(
    CommonLibSSE_NG
    GIT_REPOSITORY https://github.com/CharmedBaryon/CommonLibSSE-NG.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(CommonLibSSE_NG)
endif()

# Defines add_commonlibsse_plugin()
include("${COMMONLIBSSE_SOURCE_DIR}/cmake/CommonLibSSE.cmake")

# ----------------------------
# nlohmann_json: prefer local submodule, FetchContent fallback
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
# Plugin target (AE)
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
)

add_commonlibsse_plugin(${PLUGIN_NAME}
  SOURCES ${MMH_SOURCES}
)

target_include_directories(${PLUGIN_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  nlohmann_json::nlohmann_json
)

if (MSVC)
  target_compile_options(${PLUGIN_NAME} PRIVATE /W4 /permissive- /EHsc)
endif()
