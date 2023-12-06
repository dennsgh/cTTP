#include "parser.h"


Parser::Parser(const std::string& template_string) : template_string(template_string) {}

void Parser::addVariable(const std::string& name, const std::string& value) {
    vars[name] = value;
}

std::map<std::string, std::string> Parser::parse(const std::string& input) {
    // Create a regex from the template by replacing '{{ var }}' with '(.*)'
    std::string regex_string = std::regex_replace(template_string, std::regex("\\{\\{\\s*(\\w+)\\s*\\}\\}"), "(.*)");
    std::regex r(regex_string);

    std::smatch match;

    if (std::regex_match(input, match, r)) {
        // Create a map to store the variables
        std::map<std::string, std::string> vars;

        // Define the regex once
        std::regex var_regex("\\{\\{\\s*(\\w+)\\s*\\}\\}");

        // Iterate over the variables in the template
        std::sregex_iterator var_iter(template_string.begin(), template_string.end(), var_regex);
        std::sregex_iterator var_end;

        // Start from the second element of match as the first one is the entire string
        auto match_iter = match.begin() + 1;

        for (; var_iter != var_end; ++var_iter, ++match_iter) {
            vars[var_iter->str(1)] = *match_iter;
        }

        return vars;
    } else {
        throw std::runtime_error("Input does not match template");
    }
}

std::string Parser::removeTrailingCharacters(const std::string& line, const std::string& characters) {
    std::size_t first = line.find_first_not_of(characters);
    if (first == std::string::npos) {
        // The line contains only the specified characters
        return "";
    }
    std::size_t last = line.find_last_not_of(characters);
    return line.substr(first, (last - first + 1));
}

std::string Parser::normalize(const std::string& str) {
    std::stringstream ss(str);
    std::string to, result;
    while (std::getline(ss, to, '\n')) {
        to = removeTrailingCharacters(to, " \t\r\f\v\n"); // Remove trailing whitespace and newline characters
        if (!to.empty()) {
            if (!result.empty()) {
                result += "\n"; // Add newline character between lines, but not at the end
            }
            result += to;
        }
    }
    return result;
}

nlohmann::json Parser::json_parse(const std::string& input) {
    nlohmann::json output = fillJSONTemplate(this->template_string, input);  // Fill the template structure with values from the input string
    return output;  // Convert the filled template structure to a JSON object
}

// Method to get all sentences from a given string
std::vector<std::string> Parser::getSentences(const std::string& str) {
    std::vector<std::string> sentences;
    std::istringstream stream(str);
    std::string line;

    while (std::getline(stream, line)) {
        sentences.push_back(line);
    }

    return sentences;
}

// Method to build the regex string
std::string Parser::buildRegexString(const std::string& template_sentence, const std::string& start_delim = "{{", const std::string& end_delim = "}}") {
    if (start_delim == "{{" && end_delim == "}}") {
        std::regex r("\\{\\{\\s*(\\w+(\\.\\w+)*)\\s*\\}\\}");
        return std::regex_replace(template_sentence, r, "(.*?)");
    } else {
        // For other delimiters, avoid automatic escaping and handle them specifically.
        // This is a simplified approach. If more delimiters are added, this should be enhanced.
        std::string pattern = std::regex_replace(start_delim + "\\s*(\\w+(\\.\\w+)*)\\s*" + end_delim, std::regex(R"([\^\$\.\|\?\*\+\(\)\[\]\{\}\\])"), R"(\\$&)");
        std::regex r(pattern);
        return std::regex_replace(template_sentence, r, "(.*?)");
    }
}


// Method to extract field names from template sentence
std::vector<std::string> Parser::getFieldNames(const std::string& template_sentence) {
    std::vector<std::string> field_names;
    std::regex r("\\{\\{\\s*(\\w+(\\.\\w+)*)\\s*\\}\\}");
    std::sregex_iterator var_iter(template_sentence.begin(), template_sentence.end(), r);
    std::sregex_iterator var_end;
    
    for (; var_iter != var_end; ++var_iter) {
        field_names.push_back(var_iter->str(1));  
    }

    return field_names;
}

// Method to split a field name into parts (for nested json)
std::vector<std::string> Parser::splitFieldName(const std::string& field_name) {
    std::vector<std::string> parts;
    std::istringstream name_stream(field_name);
    std::string part;

    while (std::getline(name_stream, part, '.')) {
        parts.push_back(part);
    }

    return parts;
}

nlohmann::json Parser::fillJSONTemplate(const std::string& template_string, const std::string& input) {
    nlohmann::json json_structure = generateJSONStructure(normalize(template_string));

    auto template_sentences = getSentences(normalize(template_string));
    auto input_sentences = getSentences(normalize(input));

    size_t sentence_count = std::min(template_sentences.size(), input_sentences.size());

    for (size_t i = 0; i < sentence_count; i++) {
        fillSentence(json_structure, template_sentences[i], input_sentences[i]);
    }

    return json_structure;
}

void Parser::fillSentence(nlohmann::json& json_structure, const std::string& template_sentence, const std::string& input_sentence) {
    std::string regex_string = buildRegexString(template_sentence);
    std::regex input_regex(regex_string);

    std::smatch match;
    if (!std::regex_match(input_sentence, match, input_regex)) {
        return;
    }

    auto field_names = getFieldNames(template_sentence);

    size_t value_index = 0;
    for (const auto& field_name : field_names) {
        nlohmann::json* current = &json_structure;

        auto parts = splitFieldName(field_name);
        bool pathExists = true;

        for (size_t i = 0; i < parts.size() && pathExists; ++i) {
            const std::string& part = parts[i];
            if (!current->contains(part)) {
                pathExists = false;
                break;
            }

            if (i == parts.size() - 1) { 
                (*current)[part] = match[value_index + 1].str();
            } else {
                current = &((*current)[part]);
            }
        }

        if (pathExists) {
            ++value_index;
        }
    }
}


// Function to fill the template structure with values from the input string
Template Parser::fillTemplate(const std::string& template_string, const std::string& input) {
    std::string normalized_template_string = normalize(template_string);
    std::string normalized_input = normalize(input);
    Template template_structure = generateTemplate(normalized_template_string);
    
    std::vector<std::string> template_sentences;  // Vector to store template sentences
    std::vector<std::string> input_sentences;  // Vector to store input sentences

    std::istringstream template_stream(normalized_template_string);
    std::istringstream input_stream(normalized_input);

    std::string line;
    while (std::getline(template_stream, line)) {
        template_sentences.push_back(line);  // Split template into individual sentences
    }
    while (std::getline(input_stream, line)) {
        input_sentences.push_back(line);  // Split input into individual sentences
    }

    size_t sentence_count = std::min(template_sentences.size(), input_sentences.size());

    for (size_t i = 0; i < sentence_count; i++) {
        fillTemplateSentence(template_structure, template_sentences[i], input_sentences[i]);  // Fill each sentence in the template with the corresponding input sentence
    }
    return template_structure;
}

// Function to fill a template sentence with values from an input sentence
void Parser::fillTemplateSentence(Template& template_structure, const std::string& template_sentence, const std::string& input_sentence) {
    std::regex r("\\{\\{\\s*(\\w+(\\.\\w+)*)\\s*\\}\\}");  // Regular expression pattern to match field names in the template sentence
    std::smatch match;

    // Create a regex from the template_sentence by replacing '{{ var }}' with '(.*)'
    std::string regex_string = std::regex_replace(template_sentence, std::regex("\\{\\{\\s*(\\w+(\\.\\w+)*)\\s*\\}\\}"), "(.*)");
    std::regex input_regex(regex_string);

    if (!std::regex_match(input_sentence, match, input_regex)) {
        return; // Don't throw an error, just return
    }

    std::vector<std::string> field_names;
    std::sregex_iterator var_iter(template_sentence.begin(), template_sentence.end(), r);
    std::sregex_iterator var_end;

    // Start from the second element of match as the first one is the entire string
    auto match_iter = match.begin() + 1;

    for (; var_iter != var_end; ++var_iter, ++match_iter) {
        field_names.push_back(var_iter->str(1));  // Extract the field names from the template sentence
        //std::cout << var_iter->str(1) << " : " << *match_iter << "\n";
    }

    size_t value_index = 0;
    for (auto& field : template_structure.fields) {
        for (const auto& field_name : field_names) {
            if (field.name == field_name) {
                if (value_index < match.size() - 1) {
                    field.value = match[value_index + 1];  // Assign the matched value to the field in the template structure
                    ++value_index;
                    break;
                } else {
                    return; // If there are no enough values, just return
                }
            }
        }
    }
}

void Parser::setTemplateString(const std::string template_string){
    this->template_string = template_string;
}

// Function to parse the input and generate a JSON object


nlohmann::json Parser::generateJSONStructure(const std::string& template_string) {
    nlohmann::json output;
    std::regex r("\\{\\{\\s*(\\w+(\\.\\w+)*)\\s*\\}\\}");  // Regular expression pattern to match field names in the template string
    std::smatch match;

    std::string::const_iterator search_start(template_string.cbegin());
    while (std::regex_search(search_start, template_string.cend(), match, r)) {
        std::string field_name = match[1];  // Extract the field name from the match

        std::vector<std::string> name_parts;
        std::istringstream name_stream(field_name);
        std::string part;

        while(std::getline(name_stream, part, '.')) {
            name_parts.push_back(part);  // Split the field name by periods to handle nested JSON objects
        }

        nlohmann::json* current = &output;
        for (size_t i = 0; i < name_parts.size(); i++) {
            if (i == name_parts.size() - 1) {
                (*current)[name_parts[i]] = "";  // Default JSON value string of "", we could force type here to enforce type during filling.
            } else {
                if (!(*current).contains(name_parts[i])) {
                    (*current)[name_parts[i]] = nlohmann::json::object();  // Create a new nested JSON object if the key doesn't exist
                }
                current = &(*current)[name_parts[i]];
            }
        }

        search_start = match.suffix().first;  // Update the search start position for the next iteration
    }

    return output;  // Return the resulting JSON object
}


Template Parser::generateTemplate(const std::string& template_string) {
    Template template_structure;
    std::regex r("\\{\\{\\s*(\\w+(\\.\\w+)*)\\s*\\}\\}");  // Regular expression pattern to match field names in the template string
    std::smatch match;

    std::string::const_iterator search_start(template_string.cbegin());
    while (std::regex_search(search_start, template_string.cend(), match, r)) {
        Field field;
        field.name = match[1];  // Extract the field name from the match
        template_structure.fields.push_back(field);  // Add the field to the template structure
        search_start = match.suffix().first;  // Update the search start position for the next iteration
    }

    return template_structure;  // Return the generated template structure
}

// Function to convert a template structure to a JSON object
nlohmann::json Parser::templateToJson(const Template& template_structure) {
    nlohmann::json output;

    for (const auto& field : template_structure.fields) {
        std::vector<std::string> name_parts;
        std::istringstream name_stream(field.name);
        std::string part;

        while(std::getline(name_stream, part, '.')) {
            name_parts.push_back(part);  // Split the field name by periods to handle nested JSON objects
        }

        nlohmann::json* current = &output;
        for (size_t i = 0; i < name_parts.size(); i++) {
            if (i == name_parts.size() - 1) {
                (*current)[name_parts[i]] = field.value;  // Assign the field value to the corresponding nested key in the JSON object
            } else {
                if (!(*current).contains(name_parts[i])) {
                    (*current)[name_parts[i]] = nlohmann::json::object();  // Create a new nested JSON object if the key doesn't exist
                }
                current = &(*current)[name_parts[i]];
            }
        }
    }
    return output;  // Return the resulting JSON object
}

nlohmann::json Parser::convertTemplateFieldsToJSON(const Template& template_structure) {
    nlohmann::json output;

    for (const auto& field : template_structure.fields) {
        std::vector<std::string> name_parts;
        std::istringstream name_stream(field.name);
        std::string part;

        while(std::getline(name_stream, part, '.')) {
            name_parts.push_back(part);  // Split the field name by periods to handle nested JSON objects
        }

        nlohmann::json* current = &output;
        for (size_t i = 0; i < name_parts.size(); i++) {
            if (i == name_parts.size() - 1) {
                (*current)[name_parts[i]] = field.value;  // Assign the field value to the corresponding nested key in the JSON object
            } else {
                if (!(*current).contains(name_parts[i])) {
                    (*current)[name_parts[i]] = nlohmann::json::object();  // Create a new nested JSON object if the key doesn't exist
                }
                current = &(*current)[name_parts[i]];
            }
        }
    }

    return output;
}



