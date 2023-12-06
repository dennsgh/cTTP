#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include "anna_parser.h"

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
nlohmann::json callParser(const std::string& input, const std::string& template_string, const std::string& extension) {
    nlohmann::json vars;
    if (extension == "yaml") {
        ANNAParser p(template_string);
        vars = p.parse_logfile(input, template_string, true, { {"time", "s"}, {"byte", "MB"} }, true, true, false, false);
    } else if (extension == "json") {
        std::cerr << "Parsing .json files is not supported yet.\n";
        return vars;
    } else if (extension == "conf") {
        Parser p(template_string);
        vars = p.json_parse(input);
    } else {
        std::cerr << "Error: Unsupported file extension: " << extension << "\n";
        return vars;
    }
    return vars;

}


void processTemplateFile(const std::string& input, const std::string& template_filepath, const std::string& output_filepath) {
    std::string template_string = readFile(template_filepath);

    // Determine the file extension of the template file
    std::string extension;
    size_t extensionPos = template_filepath.find_last_of(".");
    if (extensionPos != std::string::npos) {
        extension = template_filepath.substr(extensionPos + 1);
    }

    nlohmann::json vars = callParser(input, template_string, extension);
    // Output the parsed JSON
    if (!output_filepath.empty()) {
        std::ofstream out(output_filepath);
        if (!out) {
            std::cerr << "Error: Unable to open output file: " << output_filepath << "\n";
            return;
        }
        out << std::setw(4) << vars << std::endl;
    } else {
        std::cout << std::setw(4) << vars << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Check if the number of arguments is valid
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " --template <template path> (-i <raw input> | -f <file path>) [-o <output path>]\n";
        return 1;
    }

    if (strcmp(argv[1], "--template") != 0) {
        std::cerr << "Missing --template option\n";
        return 1;
    }

    std::string template_filepath = argv[2];

    std::string input;

    if (strcmp(argv[3], "-i") == 0 && argc >= 5) {
        // If -i is specified, the next argument is the raw input string
        input = argv[4];
    } else if (strcmp(argv[3], "-f") == 0 && argc >= 5) {
        // If -f is specified, the next argument is a file path
        std::string input_filepath = argv[4];
        input = readFile(input_filepath);
    } else {
        std::cerr << "Invalid or missing input option\n";
        std::cerr << "Use -i for raw input and -f for file input\n";
        return 1;
    }

    std::string output_filepath = (argc > 6 && strcmp(argv[5], "-o") == 0) ? argv[6] : "";

    processTemplateFile(input, template_filepath, output_filepath);

    return 0;
}
