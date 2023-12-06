//anna_parser_tests.h
#ifndef ANNA_PARSER_TESTS_H
#define ANNA_PARSER_TESTS_H

#include "anna_parser.h"
#include "utils.h"
#include "test.h"

class ANNAParserTests : public TestSuite {
public:
    ANNAParserTests();

private:
    void generateTestContent(std::string& content);
    void testLoadFile();
    void testHasKey();
    void testGetNode();
    void testGetValue();
    void testMonteCarloParse();
    void _compare_content_to_expectation(nlohmann::json& content, std::vector<std::string> exceptions = {});
    void testParseLogfile();
    void testParseLogfileMultiples();
};

#endif // ANNA_PARSER_TESTS_H
