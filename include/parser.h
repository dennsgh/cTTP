#ifndef CMAKE_PARSER_H
#define CMAKE_PARSER_H
#include <map>
#include <regex>
#include <iostream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

struct Field {
    std::string name;
    std::string value;
};

struct Template {
    std::vector<Field> fields;
};
class Parser {
public:
    Parser(const std::string& template_string);
    std::map<std::string, std::string> parse(const std::string& input);
    std::string normalize(const std::string& str);
    void addVariable(const std::string& name, const std::string& value);
    nlohmann::json json_parse(const std::string& input);
    
private:
    std::string template_string;
    std::map<std::string, std::string> vars;
    Template template_structure;

    std::string removeTrailingCharacters(const std::string& line, const std::string& characters);
    Template generateTemplate(const std::string& template_string);
    void fillTemplate(Template& template_structure, const std::string& input);
    nlohmann::json templateToJson(const Template& template_structure);
    void fillSentence(Template& template_structure, const std::string& template_sentence, const std::string& input_sentence);
    
};

#endif //CMAKE_PARSER_H
