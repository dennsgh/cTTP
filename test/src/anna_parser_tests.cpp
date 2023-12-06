//anna_parser_tests.cpp
#include "anna_parser_tests.h"
#include <fstream>
#include <iostream>

ANNAParserTests::ANNAParserTests() {
    setTestSuiteName("ANNA Parser Test Suite");
    addTest("testLoadFile", [this]() { this->testLoadFile(); });
    addTest("testHasKey", [this]() { this->testHasKey(); });
    addTest("testGetNode", [this]() { this->testGetNode(); });
    addTest("testGetValue", [this]() { this->testGetValue(); });
    addTest("testParseLogfile", [this]() { this->testParseLogfile(); });
    addTest("testMonteCarloParse", [this]() { this->testMonteCarloParse(); });
    addTest("testParseLogfileMultiples", [this]() { this->testParseLogfileMultiples(); });
}
 /** TODO:

    * PORT IN ANNA TESTS**/

void ANNAParserTests::generateTestContent(std::string& content) {
    content += "issues:\n";
    content += "  active: true\n";
    content += "  examples:\n";
    content += "    - \"WARNING (SPECTRE-4056): foo bar.\":\n";
    content += "        code: SPECTRE-4056\n";
    content += "        description: foo bar.\n";
    content += "  message:\n";
    content += "    location: simulation\n";
    content += "  template: |\n";
    content += "    <group name=\"errors*\">\n";
    content += "        FATAL ({{code}}): {{description | ORPHRASE }}\n";
    content += "    </group>\n";
    content += "    <group name=\"warnings*\">\n";
    content += "        WARNING ({{code}}): {{description | ORPHRASE }}\n";
    content += "    </group>\n";
    content += "    <group name=\"errors*\">\n";
    content += "        ERROR ({{code}}): {{description | ORPHRASE }}\n";
    content += "    </group>\n";
    content += "    <group name=\"supressed*\">\n";
    content += "        {{count}} {{type}} suppressed.\n";
    content += "    </group>\n";
    content += "    <group name=\"summary*\">\n";
    content += "        spectre completes with {{num_errors | to_int }} errors, {{num_warnings | to_int }} warnings, and {{num_notices | to_int }} notices.\n";
    content += "    </group>\n";
    content += "    <group name=\"summary*\">\n";
    content += "        spectre completes with {{num_errors | to_int }} error, {{num_warnings | to_int }} warnings, and {{num_notices | to_int }} notices.\n";
    content += "    </group>\n";
}

void ANNAParserTests::testParseLogfile() {
    std::string configFileContent = R"(
        test:
          type: ttp
          message:
            location: some_location
          template: |
            My name is {{ person.name }}.
            I have a {{ person.pet.type }} named {{ person.pet.name }}.
            He is {{ person.pet.age }} years old.
          units:
            time: s
            byte: MB
    )";

    std::string logFileContent = R"(
        My name is John Doe.
        I have a Dog named Buddy.
        He is 5 years old.
    )";

    ANNAParser parser(configFileContent);
    nlohmann::json result = parser.parse_logfile(
        logFileContent,
        configFileContent,
        false,
        {{"time", "s"}, {"byte", "MB"}},
        true,
        true,
        false,  // isInputAFile
        false   // isConfigAFile
    );

    // Print the 'result' JSON object for debugging
    // std::cout << "Result: " << result.dump(4) << std::endl;

    // Perform assertions on the 'result' JSON object to verify the output

    // Example assertions
    assertEquals(true, result.contains("some_location"));
    assertEquals(true, result["some_location"].contains("test"));
    assertEquals(std::string("John Doe"), result["some_location"]["test"]["person"]["name"].get<std::string>());
    assertEquals(std::string("Dog"), result["some_location"]["test"]["person"]["pet"]["type"].get<std::string>());
    assertEquals(std::string("Buddy"), result["some_location"]["test"]["person"]["pet"]["name"].get<std::string>());
    assertEquals(std::string("5"), result["some_location"]["test"]["person"]["pet"]["age"].get<std::string>());
}

void ANNAParserTests::testParseLogfileMultiples() {
    std::string configFileContent = R"(
        test:
          type: ttp
          message:
            location: some_location
          template: |
            My name is {{ person.name }}.
            I have a {{ person.pet.type }} named {{ person.pet.name }}.
            He is {{ person.pet.age }} years old.
          units:
            time: s
            byte: MB
    )";
   
    std::string logFileContent = R"(
        My name is John Doe.
        I have a Pig named Buddy.
        He is 5 years old.
        My name is Johns Doe2.
        I have a Dog named Jammy.
        He is 15 years old.
    )";

    ANNAParser parser(configFileContent);
    nlohmann::json result = parser.parse_logfile(
        logFileContent,
        configFileContent,
        false,
        {{"time", "s"}, {"byte", "MB"}},
        true,
        true,
        false,  // isInputAFile
        false   // isConfigAFile
    );

    // Print the 'result' JSON object for debugging
    std::cout << "Result: " << result.dump(4) << std::endl;

    // Perform assertions on the 'result' JSON object to verify the output

    // Example assertions
    assertEquals(true, result.contains("some_location"));
    assertEquals(true, result["some_location"].contains("test"));
    assertEquals(std::string("John Doe"), result["some_location"]["test"]["person"]["name"].get<std::string>());
    assertEquals(std::string("Pig"), result["some_location"]["test"]["person"]["pet"]["type"].get<std::string>());
    assertEquals(std::string("Buddy"), result["some_location"]["test"]["person"]["pet"]["name"].get<std::string>());
    assertEquals(std::string("Johns Does"), result["some_location"]["test"]["person"]["name"].get<std::string>());
    assertEquals(std::string("Jammy"), result["some_location"]["test"]["person"]["pet"]["name"][1].get<std::string>());
    assertEquals(std::string("5"), result["some_location"]["test"]["person"]["pet"]["age"][0].get<std::string>());
    assertEquals(std::string("15"), result["some_location"]["test"]["person"]["pet"]["age"][1].get<std::string>());
}

void ANNAParserTests::testLoadFile() {
    std::string content = "";
    std::string filename = "test.yaml";
    generateTestContent(content);
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to create test file.");
    }
    file << content;
    file.close();

    ANNAParser parser(filename);

    nlohmann::json json = parser.getJsonNode();
    assertEquals(true, json.contains("issues"));
}

void ANNAParserTests::testHasKey() {
    std::string content = "";
    generateTestContent(content);

    ANNAParser parser(content);

    nlohmann::json json = parser.getJsonNode();

    assertEquals(true, json["issues"].contains("active"));
    assertEquals(false, json.contains("invalid_key"));
}

void ANNAParserTests::testGetNode() {
    std::string content = "";
    generateTestContent(content);

    ANNAParser parser(content);

    nlohmann::json json = parser.getJsonNode();

    const nlohmann::json& node = json["issues"]["message"]["location"];

    assertEquals(std::string("simulation"), node.get<std::string>());
}

void ANNAParserTests::testGetValue() {
    std::string content = "";
    generateTestContent(content);

    ANNAParser parser(content);

    nlohmann::json json = parser.getJsonNode();

    std::string code = json["issues"]["examples"][0]["WARNING (SPECTRE-4056): foo bar."]["code"].get<std::string>();
    std::string description = json["issues"]["examples"][0]["WARNING (SPECTRE-4056): foo bar."]["description"].get<std::string>();

    assertEquals(std::string("SPECTRE-4056"), code);
    assertEquals(std::string("foo bar."), description);
}

void ANNAParserTests::_compare_content_to_expectation(nlohmann::json& content, std::vector<std::string> exceptions) {
    for (auto& inventory : {"output_statements", "analysis_control_inventory", "global_user_options"}) {
        auto& statement_container = content["simulation"][inventory];
        assert(statement_container.is_array());
        auto& statement = statement_container[0];
        assert(statement.is_object());
        assert(statement.count("name") > 0 || statement.count("element") > 0);
        assert(statement.count("count") > 0 || statement.count("value") > 0);
        assert(content["simulation"]["simulation"]["CPU"][0]["cpu_type_info"].is_string());
        bool test_memory_used = true;
        if (!exceptions.empty() && std::find(exceptions.begin(), exceptions.end(), "Memory") != exceptions.end()) {
            test_memory_used = false;
        }
        if (test_memory_used) {
            assert(content["simulation"]["simulation"]["Memory"][0]["amount"].is_number());
        }
    }
}

void ANNAParserTests::testMonteCarloParse() {
    std::string tests_dir = "./testfiles";
    std::string logFileContent = load_file(tests_dir + "/logs_28nm_tb_lpf_flf_sfdr/nocali_aps_mod_mt2/1_HT.log");
    std::string configFileContent = load_file(tests_dir + "/SPECTRE_CFG");
    ANNAParser parser(configFileContent);
    nlohmann::json content = parser.parse_logfile(
        logFileContent,
        configFileContent,
        false,
        {{"time", "s"}, {"byte", "MB"}},
        true,
        true,
        false,  // isInputAFile
        false   // isConfigAFile
    );
    std::cout << content.dump(4) << std::endl; // Print JSON with 4 space indent
    _compare_content_to_expectation(content, {"Memory"});
    assert(content["simulation"].count("circuit_inventory") > 0);
    assert(content["simulation"]["circuit_inventory"].is_array());
    assert(content["simulation"].count("issues") > 0);
    assert(3588 == content["simulation"]["analyses"]["dc"]["convergence"]["iterations"].get<int>());
    content = parser.parse_logfile(logFileContent, configFileContent);
    // std::cout << content.dump(4) << std::endl;
    _compare_content_to_expectation(content);
    assert(17 == content["simulation"]["circuit_inventory"].size());
}
