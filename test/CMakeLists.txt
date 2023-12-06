cmake_minimum_required(VERSION 3.2)
project(parser_tests)

include_directories(${PARSER_HEADERS_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect all source files from the src subdirectory
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add the main.cpp file
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Create the executable for the test suite
add_executable(
    parser_tests ${SOURCE_FILES}
    src/utils.cpp
)

# Link against the PARSER library and nlohmann_json
target_link_libraries(parser_tests PARSER ANNA_PARSER nlohmann_json::nlohmann_json fmt::fmt-header-only)

# Install the test suite executable to the 'bin' directory
install(TARGETS parser_tests DESTINATION bin)
