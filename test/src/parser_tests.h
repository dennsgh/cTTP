#ifndef PARSER_TEST_H
#define PARSER_TEST_H

#include "parser.h"
#include "test.h"
#include "utils.h"
#include <vector>

 /** TODO:
    * IMPLEMENT GROUPS
    * IMPLEMENT * OPERATOR
    * IMPLEMENT ORPHRASE
    * PORT IN ANNA TESTS**/

class ParserTests : public TestSuite {
public:
    ParserTests();

private:
    void testLineParse();
    void testParseNestedGroups();
    void testNormalize();
    void testNormalizedParse();
    void testTTPParsing();
};

#endif