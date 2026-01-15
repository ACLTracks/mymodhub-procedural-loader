cmake_minimum_required(VERSION 3.26)

project(mymodhub_procedural_loader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PLUGIN_NAME "mymodhub-procedural-loader")

# ----------------------------
# Dependencies
# ----------------------------

# Disable json tests globally
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

# CommonLibSSE-NG (AE runtime)
add_subdirectory(extern/CommonLibSSE-NG)

# nlohmann_json: local submodule preferred, FetchContent fallback
set(NLOHMANN_JSON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann_json")
if (EXISTS "${NLOHMANN_JSON_SOURCE_DIR}/CMakeLists.txt")
  add_subdirectory("${NLOHMANN_JSON_SOURCE_DIR}")
else()
  include(FetchContent)
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
  src/mmh/PackPaths.cpp
  src/mmh/PackLoader.cpp
  src/mmh/Validate.cpp
  src/mmh/Apply.cpp
  src/racemenu/RaceMenuBridge.cpp
)

# This macro wires SKSE + AE correctly
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
