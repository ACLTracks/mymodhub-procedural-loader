cmake_minimum_required(VERSION 3.26)
project(mymodhub_procedural_loader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output name
set(PLUGIN_NAME "mymodhub-procedural-loader")

# Dependencies
find_package(nlohmann_json CONFIG REQUIRED)

# CommonLibSSE NG (planned as submodule)
# After adding the submodule, this should work:
# add_subdirectory(extern/CommonLibSSE)
# and link against CommonLibSSE::CommonLibSSE

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
  nlohmann_json::nlohmann_json
)

# When CommonLibSSE NG is added:
# target_link_libraries(${PLUGIN_NAME} PRIVATE CommonLibSSE::CommonLibSSE)

# Windows niceties
if (MSVC)
  target_compile_options(${PLUGIN_NAME} PRIVATE /W4 /permissive- /EHsc)
endif()
