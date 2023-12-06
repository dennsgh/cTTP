# parser/CMakeLists.txt
cmake_minimum_required(VERSION 3.2)
project(PARSER CXX)

set(SOURCE_FILES
    parser.cpp
)

add_library(PARSER SHARED ${SOURCE_FILES})

# Link directly to the nlohmann_json library
target_link_libraries(PARSER PUBLIC nlohmann_json::nlohmann_json yaml-cpp)
# Include the include directories of the nlohmann/json library
target_include_directories(PARSER INTERFACE ${json_SOURCE_DIR})

install(TARGETS PARSER DESTINATION ${PARSEABLE_INSTALL_LIB_DIR})
install(FILES parser.h DESTINATION ${PARSEABLE_INSTALL_INCLUDE_DIR})
