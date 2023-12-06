//utils.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "utils.h"

void prettyPrintJson(const nlohmann::json& json) {
    std::cout << json.dump(4) << std::endl;
}

std::string load_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
