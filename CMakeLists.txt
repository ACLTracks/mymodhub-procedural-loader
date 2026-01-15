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

# Provide spdlog (and fmt) via FetchContent so CommonLibSSE-NG's find_package(spdlog CONFIG) succeeds
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# CommonLibSSE-NG: prefer submodule, fallback to FetchContent (pin a commit/tag if you want hard reproducibility)
set(COMMONLIBSSE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE-NG")
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

# nlohmann_json: prefer local submodule, FetchContent fallback
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

# This macro wires SKSE + AE correctly (provided by CommonLibSSE-NG)
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
