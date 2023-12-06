# cTTP Parser
A command-line program for parsing input using a template and generating JSON output.

This command-line program is designed to parse input using a template and generate either a JSON output or simple stdout.
It provides flexibility in accepting input from both raw input strings and files, allowing for easy integration into various workflows.

## Usage
```shell
cttp_parser --template <template_path> (-i <raw_input> | -f <file_path>) [-o <output_path>]
```
- template_path: The path to the template file.
- raw_input: A raw input string (used with the -i option).
- file_path: The path to the input file (used with the -f option).
- output_path (optional): The path to the output file. If not specified, the JSON output will be printed to the console.

The program will parse the input based on the provided template and generate a JSON output.
Here's an example command:

```
./cttp_parser --template template.txt -i "Hello my name is Alice!" -o output.json
```

with the content of template.txt being:
```
Hello my name is {{}}!
```
## Requirements
C++ compiler supporting C++11 or later
The nlohmann/json.hpp library
## Running and Building

You could manually build this.
```
mkdir build
cd build
cmake ..
make
```

## Notes

The Parser is a module that facilitates template parsing and JSON conversion. It provides functionality to generate a template structure from a template string, fill the template structure with values from an input string, and convert the filled template structure to a JSON object.

### Functionality

1. **Generate Template**: The `generateTemplate` function takes a template string as input and generates a template structure. It extracts field names from the template string using regular expressions and stores them in the template structure.

2. **Fill Template**: The `fillTemplate` function fills the template structure with values from an input string. It splits the template and input strings into individual sentences and matches the corresponding sentences. The template sentences and input sentences should align. If the number of sentences in the template and input does not match, an error is thrown. The `fillSentence` function is called to fill each sentence in the template with the corresponding input sentence.

3. **Fill Sentence**: The `fillSentence` function fills a template sentence with values from an input sentence. It uses regular expressions to match field names in the template sentence. It checks if the input sentence matches the template sentence pattern. If not, an error is thrown. It extracts field names from the template sentence and matches them with values from the input sentence. The filled values are stored in the template structure.

4. **JSON Parsing**: The `json_parse` function parses the input string and generates a JSON object. It first generates a template structure by calling `generateTemplate` with the template string. It then fills the template structure with values from the input string by calling `fillTemplate`. Finally, it converts the filled template structure into a JSON object by calling `templateToJson` and returns the result.

5. **Template to JSON**: The `templateToJson` function converts a template structure to a JSON object. It iterates over the template structure fields and handles nested JSON objects. It splits field names by periods and assigns values to the corresponding nested keys in the JSON object.

### Dependencies

The Template Parser module relies on the following libraries: `<regex>`, `<vector>`, `<string>`, `<stdexcept>`, `<sstream>`, and `nlohmann::json` (a JSON library).

## License
This project is licensed under the MIT License.

Please make sure to include the necessary header files, dependencies, and adjust the compilation commands according to your specific setup.