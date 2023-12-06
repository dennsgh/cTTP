# parser/CMakeLists.txt
cmake_minimum_required(VERSION 3.2)
project(ANNA_PARSER CXX)

# Add the source files for the ANNA_PARSER library
set(SOURCE_FILES
    anna_parser.cpp
)

# Build the ANNA_PARSER library
add_library(ANNA_PARSER SHARED ${SOURCE_FILES})

# Link against the nlohmann_json and yaml-cpp libraries
target_link_libraries(ANNA_PARSER PUBLIC nlohmann_json::nlohmann_json yaml-cpp)

# Specify the include directories for the ANNA_PARSER library
target_include_directories(ANNA_PARSER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Install the ANNA_PARSER library and header files
install(TARGETS ANNA_PARSER DESTINATION ${PARSEABLE_INSTALL_LIB_DIR})
install(FILES anna_parser.h DESTINATION ${PARSEABLE_INSTALL_INCLUDE_DIR})
