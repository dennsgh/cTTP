#include "parser_tests.h"

#include <iostream>
#include <exception>
#include <functional>
#include <sstream>
#include <stdexcept>


ParserTests::ParserTests() {
    setTestSuiteName("Core Parser Test Suite");
    addTest("testLineParse", [this]() { this->testLineParse(); });
    addTest("testParseNestedGroups", [this]() { this->testParseNestedGroups(); });
    addTest("testNormalize", [this]() { this->testNormalize(); });
    addTest("testNormalizedParse", [this]() { this->testNormalizedParse(); });
    addTest("testTTPParsing", [this]() { this->testTTPParsing(); });
}

void ParserTests::testTTPParsing() {
    std::string template_string = R"(
        <group name="errors*">
            FATAL ({{code}}): {{description | ORPHRASE }}
        </group>
        <group name="warnings*">
            WARNING ({{code}}): {{description | ORPHRASE }}
        </group>
        <group name="errors*">
            ERROR ({{code}}): {{description | ORPHRASE }}
        </group>
        <group name="supressed*">
            {{count}} {{type}} suppressed.
        </group>
        <group name="summary*">
            spectre completes with {{num_errors | to_int }} errors, {{num_warnings | to_int }} warnings, and {{num_notices | to_int }} notices.
        </group>
        <group name="summary*">
            spectre completes with {{num_errors | to_int }} error, {{num_warnings | to_int }} warnings, and {{num_notices | to_int }} notices.
        </group>
    )";

    std::string input = R"(
        WARNING (SPECTRE-4108): I31.M7.pw1: Instance I31.M7, Expression `abs(re(I(s)*re(V(d,s))))' having value 81.9331e-03 has returned to within bounds [-340.282e+36, 82.0492e-03]. Peak value was 438.246e-03 at time 41.8462e-03. Total duration of overshoot was 728.877e-09.

        spectre completes with 0 errors, 9 warnings, and 26 notices.
    )";

    Parser parser(template_string);
    auto vars = parser.json_parse(input);

    nlohmann::json expected = {
        {"warnings", {
            {
                {"code", "SPECTRE-4108"},
                {"description", "I31.M7.pw1: Instance I31.M7, Expression `abs(re(I(s)*re(V(d,s))))' having value 81.9331e-03 has returned to within bounds [-340.282e+36, 82.0492e-03]. Peak value was 438.246e-03 at time 41.8462e-03. Total duration of overshoot was 728.877e-09."}
            }
        }},
        {"summary", {
            {
                {"num_errors", 0},
                {"num_warnings", 9},
                {"num_notices", 26}
            }
        }}
    };

    nlohmann::json actual = vars;

    assertEquals(expected, actual);
}



void ParserTests::testLineParse() {
    std::string input = "Hello, my name is Alice and I am 25 years old";
    std::string template_string = "Hello, my name is {{ name }} and I am {{ age }} years old";
    Parser parser(template_string);
    nlohmann::json vars = parser.json_parse(input);
    std::cout << vars.dump() << std::endl;
    assertEquals(std::string("Alice"), std::string(vars["name"]));
    assertEquals(std::string("25"), std::string(vars["age"]));
}


void ParserTests::testParseNestedGroups() {
    std::string input = "My name is Bob.\nI have a dog named Rover.\nHe is 3 years old.";
    std::string template_string = "My name is {{ person.name }}.\nI have a {{ person.pet.type }} named {{ person.pet.name }}.\nHe is {{ person.pet.age }} years old.";
    Parser parser(template_string);
    auto vars = parser.json_parse(input);

    nlohmann::json expected = {
        {"person", {
            {"name", "Bob"},
            {"pet", {
                {"type", "dog"},
                {"name", "Rover"},
                {"age", "3"}
            }}
        }}
    };

    nlohmann::json actual = vars;

    assertEquals(expected, actual);
}


void ParserTests::testNormalizedParse() {

    std::string input = " \t\n\r\f\v Hello, my name is Alice.\n I am 25 years old. \t\n\r\f\v";
    std::string template_string = " \t\n\r\f\v Hello, my name is {{ name }}.\n I am {{ age }} years old. \t\n\r\f\v";
    Parser parser(template_string);
    auto vars = parser.json_parse(input);
    
    assertEquals(std::string("Alice"), std::string(vars["name"]));
    assertEquals(std::string("25"), std::string(vars["age"]));
}


void ParserTests::testNormalize() {
    std::string input = " \t\n\r\f\v Hello, my name is Alice.\n I am 25 years old. \t\n\r\f\v";
    std::string template_string = " \t\n\r\f\v Hello, my name is {{ name }}.\n I am {{ age }} years old. \t\n\r\f\v";

    Parser parser(template_string);
    auto trimmed_template = parser.normalize(template_string);
    auto trimmed_input = parser.normalize(input);

    assertEquals(std::string("Hello, my name is Alice.\nI am 25 years old."), std::string(trimmed_input));
    assertEquals(std::string("Hello, my name is {{ name }}.\nI am {{ age }} years old."), std::string(trimmed_template));
}