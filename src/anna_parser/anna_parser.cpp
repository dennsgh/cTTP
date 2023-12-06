#include "anna_parser.h"
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <regex>
#include <filesystem>



ANNAParser::ANNAParser(const std::string& source) {
    if (isFile(source)) {
        loadFile(source);
    } else {
        loadFromString(source);
    }
}
std::vector<std::string> ANNAParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}



bool ANNAParser::isFile(const std::string& source) const {
    std::ifstream file(source);
    return file.good();
}

void ANNAParser::loadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    yamlNode = YAML::Load(file);
}

void ANNAParser::loadFromString(const std::string& yamlString) {
    yamlNode = YAML::Load(yamlString);
}

nlohmann::json ANNAParser::load_config_from_file(const std::string& filename) {
    if (filename.substr(filename.find_last_of(".") + 1) == "json") {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open " + filename);
        }
        nlohmann::json config = nlohmann::json::parse(file);
        file.close();
        preprocess_config(config);
        return config;
    }
    else if (filename.substr(filename.find_last_of(".") + 1) == "yaml" || filename.substr(filename.find_last_of(".") + 1) == "yml") {
        YAML::Node yamlNode = YAML::LoadFile(filename);
        nlohmann::json config = yamlToJSON(yamlNode);
        preprocess_config(config);
        return config;
    }
    else {
        throw std::invalid_argument("File type not supported for " + filename);
    }
}

nlohmann::json ANNAParser::load_config_from_string(const std::string& yamlString) {
    YAML::Node yamlNode = YAML::Load(yamlString);
    nlohmann::json config = yamlToJSON(yamlNode);
    preprocess_config(config);
    return config;
}
bool ANNAParser::hasKey(const std::string& key) const {
    return yamlNode[key];
}

const YAML::Node& ANNAParser::getNode(const std::string& key) const {
    static YAML::Node nullNode; // Define a static null node to return in case key not found
    const YAML::Node& node = yamlNode[key];
    if (node.IsNull()) {
        std::cerr << "Key not found: " << key << std::endl;
        return nullNode;
    }
    return node;
}

nlohmann::json ANNAParser::yamlToJSON(const YAML::Node& node) const {
    nlohmann::json json;
    if (node.IsScalar()) {
        json = node.Scalar();
    } else if (node.IsSequence()) {
        for (const auto& element : node) {
            json.push_back(yamlToJSON(element));
        }
    } else if (node.IsMap()) {
        for (const auto& pair : node) {
            json[pair.first.Scalar()] = yamlToJSON(pair.second);
        }
    }
    return json;
}

nlohmann::json ANNAParser::getJsonNode() const {
    return yamlToJSON(yamlNode);
}

nlohmann::json ANNAParser::legacy_load(const std::string& fn, bool isFile) {
    nlohmann::json config;
    if (isFile) {
        return load_config_from_file(fn);
    } else {
        return load_config_from_string(fn);
    }
    return config;
}
//Legacy function to setup config files
void ANNAParser::preprocess_config(nlohmann::json& config) {
    for (auto& element : config.items()) {
        nlohmann::json& field_description = element.value();
        if (field_description["type"] == "ttp") {
            std::replace(field_description["template"].begin(), field_description["template"].end(), '\n', ' ');
        }
    }
}



nlohmann::json ANNAParser::parse_logfile(
    const std::string& input, 
    const std::string& fn_cfg, 
    bool normalize /*= true*/, 
    std::unordered_map<std::string, std::string> force_units /*= { {"time", "s"}, {"byte", "MB"} }*/, 
    bool match /*= true*/, 
    bool regex /*= true*/,
    bool isInputAFile /*= true*/,
    bool isConfigAFile /*= true*/) {
    
    nlohmann::json content;

    nlohmann::json config = this->legacy_load(fn_cfg,isConfigAFile);
    std::string log_file_data;
    if (isInputAFile) {
        std::ifstream log_file(input);
        if (!log_file) {
            throw std::runtime_error("Failed to open log file: " + input);
        }
        log_file_data = std::string(std::istreambuf_iterator<char>(log_file), std::istreambuf_iterator<char>());
    } else {
        log_file_data = input;
    }

    for (auto& element : config.items()) {
        std::string field = element.key();
        nlohmann::json field_description = element.value();
        if (field_description["type"] == "exact_match" || field_description["type"] == "regex") {
            std::istringstream iss(log_file_data);
            std::vector<std::string> log_file_lines{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
            for (size_t line_number = 0; line_number < log_file_lines.size(); line_number++) {
                std::string line = log_file_lines[line_number];
                if (field_description["type"] == "exact_match" && match) {
                    std::string match_str = field_description["match"];
                    if (line.find(match_str) != std::string::npos) {
                        process_match(content, line, field_description, field);
                    }
                }
                else if (field_description["type"] == "regex" && regex) {
                    std::string match_str = field_description["match"];
                    std::regex pattern(match_str);
                    if (std::regex_search(line, pattern)) {
                        process_match(content, line, field_description, field);
                    }
                }
            }
        } else if (field_description["type"] == "ttp") {
            Parser p = Parser(field_description["template"].get<std::string>());
            nlohmann::json x = p.json_parse(log_file_data);
            //std::cout << log_file_data << std::endl;
            //std::cout << x.dump(4) << std::endl;
            //std::cout << field_description << std::endl;
            store_at_message_location(content, x, field_description, field);
        }
            //std::cout << content <<std::endl;
    }
    return content;
}
std::vector<std::vector<std::string>> ANNAParser::pathfinder(
    nlohmann::json& content, 
    const std::vector<std::string>& path, 
    std::vector<std::vector<std::string>> paths, 
    std::string query
) {
    std::vector<std::string> _path = path;
    if(content.is_object()) {
        for(auto& el : content.items()) {
            auto new_path = path;
            new_path.push_back(el.key());
            paths = pathfinder(el.value(), new_path, paths, query);
        }
    } else if(content.is_array()) {
        int i = 0;
        for(auto& el : content) {
            auto new_path = path;
            new_path.push_back(std::to_string(i));
            paths = pathfinder(el, new_path, paths, query);
            i++;
        }
    } else {
        if(path.back().find(query) != std::string::npos) {
            paths.push_back(path);
        }
    }
    return paths;
}

nlohmann::json ANNAParser::normalize_units(nlohmann::json& parser_result, std::unordered_map<std::string, std::string> conf_units) {
    std::vector<std::vector<std::string>> pathlist = pathfinder(parser_result);
    if(pathlist.empty()) {
        return parser_result;
    }

    std::string t_unit = conf_units["time"], d_unit = conf_units["byte"];
    for(auto& path : pathlist) {
        auto value_path = path;
        value_path.pop_back();
        value_path.push_back(path.back().substr(0, path.back().length() - 5)); // remove last two keys to get value path

        std::string original_unit = this->get_value_by_path(parser_result, path);
        std::string original_value = this->get_value_by_path(parser_result, value_path);

        float multiplier;
        std::string new_unit;
        if (original_unit.find('b') != std::string::npos || original_unit.find("byt") != std::string::npos && original_unit != "s") {
            std::string unit_key(1, original_unit[0]);
            std::string d_unit_key(1, d_unit[0]);
            multiplier = pow(10, (metric[unit_key] - metric[d_unit_key]));
            new_unit = d_unit;
        } else if (original_unit.find('s') != std::string::npos) {
            std::string unit_key(1, original_unit[0]);
            std::string t_unit_key(1, t_unit[0]);
            multiplier = pow(10, (metric[unit_key] - metric[t_unit_key]));
            new_unit = t_unit;
        }
        float new_value = std::stof(original_value) * multiplier;
        this->set_value_by_path(parser_result, path, new_unit);
        this->set_value_by_path(parser_result, value_path, std::to_string(new_value));
    }

    return parser_result;
}


void ANNAParser::store_at_message_location(nlohmann::json& container, const nlohmann::json& value, const nlohmann::json& field_description, const std::string& field) {
    const std::string& json_path = field_description["message"]["location"];

    std::vector<std::string> keys = split(json_path, '.');

    // Point location to the correct place in the container
    nlohmann::json* location = &container;
    for (const std::string& key : keys) {
        if (location->find(key) == location->end()) {
            // Create intermediate key if it doesn't exist
            (*location)[key] = nlohmann::json::object();
        }
        location = &(*location)[key];
    }

    // Process the value to be inserted
    if (field_description["message"].contains("mapping")) {
        const std::string& mapping_value = field_description["message"]["mapping"][value.get<std::string>()];
        (*location)[field] = mapping_value;  // Assign to location
    }
    else {
        if (value.is_object()) {
            (*location)[field].update(value);  // Update the field within location
        }
        else {
            (*location)[field] = value;  // Assign to location
        }
    }
}

std::string ANNAParser::get_value_by_path(nlohmann::json& content, std::vector<std::string> path) {
    nlohmann::json* node = &content;
    for(auto& key : path) {
        if(node->is_object()) {
            node = &(*node)[key];
        } else if(node->is_array()) {
            node = &(*node)[std::stoi(key)];
        }
    }
    return *node;
}

void ANNAParser::set_value_by_path(nlohmann::json& content, std::vector<std::string> path, std::string value) {
    nlohmann::json* node = &content;
    for(auto& key : path) {
        if(node->is_object()) {
            node = &(*node)[key];
        } else if(node->is_array()) {
            node = &(*node)[std::stoi(key)];
        }
    }
    *node = value;
}

void ANNAParser::process_match(nlohmann::json& content, const std::string& line, const nlohmann::json& field_description, const std::string& field) {
    std::string data;
    if (field_description["type"] == "exact_match") {
        if (field_description.find("exists") != field_description.end()) {
            data = "true";
        }
        // TODO: Callback was empty, do we need it?
        else {
            data = line;
            std::istringstream iss(data);
            std::vector<std::string> data_split{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
            if (field_description.find("position") != field_description.end()) {
                data = data_split[field_description["position"]];
            }
            if (field_description["convert"] == "int") {
                data = std::to_string(std::stoi(data));
            }
            else if (field_description["convert"] == "float") {
                data = std::to_string(std::stof(data));
            }
        }
    }
    else if (field_description["type"] == "regex") {
        std::regex match_regex(field_description["match"]);
        std::smatch match_result;
        if (std::regex_search(line, match_result, match_regex)) {
            data = match_result[field_description["position"]];
            if (field_description["convert"] == "int") {
                data = std::to_string(std::stoi(data));
            }
            else if (field_description["convert"] == "float") {
                data = std::to_string(std::stof(data));
            }
        }
    }
    store_at_message_location(content, data, field_description, field);
}

