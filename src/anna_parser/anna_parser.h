#ifndef ANNA_PARSER_H
#define ANNA_PARSER_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "parser.h"
#include <nlohmann/json.hpp>


class ANNAParser {
public:
    ANNAParser(const std::string& filename ="");

    bool hasKey(const std::string& key) const;
    const YAML::Node& getNode(const std::string& key) const;
    void loadFromString(const std::string& yamlString);
    bool isFile(const std::string& source) const;
    template <typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    nlohmann::json yamlToJSON(const YAML::Node& node) const;

    nlohmann::json getJsonNode() const;

    nlohmann::json parse_logfile(
        const std::string& fn_log, 
        const std::string& fn_cfg=nullptr, 
        bool normalize = true, 
        std::unordered_map<std::string, std::string> force_units = { {"time", "s"}, {"byte", "MB"} }, 
        bool match = true, 
        bool regex = true,
        bool isInputAFile = true,
        bool isConfigAFile = true);

    std::unordered_map<std::string, int> metric = {
        {"k", 3},        // kilo
        {"M", 6},        // mega
        {"G", 9},        // giga
        {"K", 3},
        {"h", 2},        // hecto
        {" ", 0},        // default !
        {"s", 0},        // default !
        {"sec", 0},      // default !
        {"seconds", 0},  // default !
        {"second", 0},   // default !
        {"0", 0},        // default !
        {"b", 0},        // default ! // the main dimension unit will be subject to .lower()!
        {"byte", 0},     // default !
        {"bytes", 0},    // default !
        {"d", -1},       // deci
        {"c", -2},       // centi
        {"m", -3},       // milli
        {"u", -6},       // micro
        {"n", -9}        // nano
    };

    nlohmann::json load_config_from_string(const std::string& yamlString);
    nlohmann::json load_config_from_file(const std::string& filename);

private:
    YAML::Node yamlNode;


    std::vector<std::string> split(const std::string& str, char delimiter);

    void preprocess_config(nlohmann::json& config);

    nlohmann::json legacy_load(const std::string& fn, bool isFile=true);
    void process_match(nlohmann::json& content, const std::string& line, const nlohmann::json& field_description, const std::string& field);
    void loadFile(const std::string& filename);
    void store_at_message_location(nlohmann::json& container, const nlohmann::json& value, const nlohmann::json& field_description, const std::string& field);


    std::vector<std::vector<std::string>> pathfinder(
        nlohmann::json& content, 
        const std::vector<std::string>& path = {}, 
        std::vector<std::vector<std::string>> paths = {}, 
        std::string query = "_unit"
    );

    nlohmann::json normalize_units(nlohmann::json& parser_result, std::unordered_map<std::string, std::string> conf_units);

    std::string get_value_by_path(nlohmann::json& content, std::vector<std::string> path);
    void set_value_by_path(nlohmann::json& content, std::vector<std::string> path, std::string value);



};

// Include the template definition in the header file
template <typename T>
T ANNAParser::getValue(const std::string& key, const T& defaultValue) const {
    if (yamlNode[key]) {
        try {
            return yamlNode[key].as<T>();
        } catch (YAML::Exception&) {
            std::cerr << "Failed to retrieve value for key: " << key << std::endl;
        }
    }
    return defaultValue;
}

#endif  // ANNA_PARSER_H