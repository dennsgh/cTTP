cmake_minimum_required(VERSION 3.2)

project(cTTP_Parser)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.10.4  
)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.6.3 
)

# Add fmt library
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 8.0.1
)

FetchContent_MakeAvailable(json yaml-cpp fmt)


FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
  FetchContent_Populate(yaml-cpp)
  add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(PARSEABLE_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PARSEABLE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(PARSEABLE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(PARSER_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/parser)

include_directories(${PARSEABLE_INSTALL_INCLUDE_DIR})
include_directories(${PARSER_HEADERS_DIR})

add_subdirectory(src)
add_subdirectory(test)
