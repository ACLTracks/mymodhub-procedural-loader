cmake_minimum_required(VERSION 3.26)

project(mymodhub_procedural_loader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output name
set(PLUGIN_NAME "mymodhub-procedural-loader")

# Disable json tests globally (covers both add_subdirectory and FetchContent cases)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

# CommonLibSSE-NG (as submodule)
# Expected path: extern/CommonLibSSE-NG
add_subdirectory(extern/CommonLibSSE-NG)

# nlohmann_json: prefer local external/nlohmann_json if present, else FetchContent fallback
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

add_library(${PLUGIN_NAME} SHARED
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

target_compile_definitions(${PLUGIN_NAME} PRIVATE
  PLUGIN_NAME="${PLUGIN_NAME}"
)

target_include_directories(${PLUGIN_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  CommonLibSSE::CommonLibSSE
  nlohmann_json::nlohmann_json
)

# Windows niceties
if (MSVC)
  target_compile_options(${PLUGIN_NAME} PRIVATE /W4 /permissive- /EHsc)
endif()
