cmake_minimum_required(VERSION 3.2)
project(CTTP_PARSER)

add_subdirectory(parser)
add_subdirectory(anna_parser)
set(SOURCE_FILES main.cpp)

add_executable(cttp_parser ${SOURCE_FILES})
target_link_libraries(cttp_parser PARSER ANNA_PARSER nlohmann_json::nlohmann_json)

install(TARGETS cttp_parser DESTINATION ${PARSEABLE_INSTALL_BIN_DIR})
