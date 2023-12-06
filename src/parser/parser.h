#ifndef CMAKE_PARSER_H
#define CMAKE_PARSER_H
#include <map>
#include <regex>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "nlohmann/json.hpp"

//======================OBSOLETE========================//
struct Field {
    std::string name;
    std::string value;
};

struct Template {
    std::vector<Field> fields;
};
//======================OBSOLETE========================//
class Parser {
public:
    Parser(const std::string& template_string);
    std::map<std::string, std::string> parse(const std::string& input);
    std::string normalize(const std::string& str);
    void addVariable(const std::string& name, const std::string& value);
    nlohmann::json json_parse(const std::string& input);
    std::string template_string;
    std::string trim(const std::string& str);
    std::string regex_escape(const std::string& s) {
        static const std::regex escape(R"([\^\$\.\|\?\*\+\(\)\{\}\[\]])"); // list of special characters
        static const std::string replacement(R"(\\&)");
        
        return std::regex_replace(s, escape, replacement);
    }

private:
    std::vector<std::string> getSentences(const std::string& str);
    std::vector<std::string> splitFieldName(const std::string& field_name);
    std::vector<std::string> getFieldNames(const std::string& template_sentence);
    std::string buildRegexString(const std::string& template_sentence, const std::string& start_delim, const std::string& end_delim);
    std::map<std::string, std::string> vars;
    Template template_structure;
    nlohmann::json json_structure;
    void setTemplateString(const std::string template_string);
    nlohmann::json convertTemplateFieldsToJSON(const Template& template_structure);
    void fillGroup(nlohmann::json& json_structure, const std::string& content, const std::string& group_name = "");

    std::string removeTrailingCharacters(const std::string& line, const std::string& characters);
    Template generateTemplate(const std::string& template_string);
    nlohmann::json fillJSONTemplate(const std::string& template_string, const std::string& input);
    Template fillTemplate(const std::string& template_string, const std::string& input);
    nlohmann::json templateToJson(const Template& template_structure);
    void fillSentence(nlohmann::json& json_structure, const std::string& template_sentence, const std::string& input_sentence);
    void fillTemplateSentence(Template& template_structure, const std::string& template_sentence, const std::string& input_sentence);
    nlohmann::json generateJSONStructure(const std::string& template_string);
};

#endif //CMAKE_PARSER_H
