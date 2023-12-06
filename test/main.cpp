#include "parser_tests.h"
#include "anna_parser_tests.h"

int main() {
    ParserTests parserTests;
    parserTests.runTests();
    
    ANNAParserTests anna_tests;
    anna_tests.runTests();
    
    return 0;
}