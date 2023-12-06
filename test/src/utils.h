//utils.h
#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <nlohmann/json.hpp>

void prettyPrintJson(const nlohmann::json& json);
std::string load_file(const std::string& filename);

#endif  // JSON_UTILS_H