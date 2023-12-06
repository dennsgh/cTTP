#include "test.h"
#include <iostream>
#include <iomanip>
#include <fmt/color.h>

bool Test::Run(const std::string& annotation) {
    bool testPassed = false;
    std::stringstream capturedOut;
    std::streambuf* coutBuf = std::cout.rdbuf();
    std::cout.rdbuf(capturedOut.rdbuf());

    std::stringstream capturedErr;
    std::streambuf* cerrBuf = std::cerr.rdbuf();
    std::cerr.rdbuf(capturedErr.rdbuf());

    try {
        testFunc();
        testPassed = true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        capturedStderr = capturedErr.str();
        if (annotation.empty() && !testName.empty()) {
            capturedStderr = fmt::format("[Test Suite: '{}', Test Case: '{}']\n{}", annotation, testName, capturedStderr);
        }
    }

    std::cout.rdbuf(coutBuf);
    std::cerr.rdbuf(cerrBuf);

    capturedStdout = capturedOut.str();

    return testPassed;
}

std::string Test::getName() {
    return testName;
}
std::string Test::getCapturedStdout(){
    return capturedStdout;
}
std::string Test::getCapturedStderr(){
    return capturedStderr;
}


TestSuite::TestSuite() : successCount(0), failureCount(0), testSuiteName("") {}

void TestSuite::runTests() {
    fmt::print(fg(fmt::color::blue), "xxxxxxxxxxxxxxxxxxxxxxxxxx-{}-xxxxxxxxxxxxxxxxxxxxxxxxxx\n",testSuiteName);
    fmt::print("Running tests for '{}'\n", testSuiteName);

    for (auto& test : tests) {
        fmt::print("Running test: {} ", test.getName());
        std::cout.flush(); // Ensure that the dots are printed immediately

        if (test.Run(testSuiteName)) {
            successCount++;
            fmt::print(fg(fmt::color::green), ".\n");
        } else {
            failureCount++;
            fmt::print(fg(fmt::color::red), "F\n");
        }
    }

    fmt::print(fg(fmt::color::green), "=========================================================================\n");
    fmt::print("Test summary for '{}':\n", testSuiteName);
    fmt::print("Total tests run: {}\n", tests.size());
    fmt::print("Passed: ");
    fmt::print(fg(fmt::color::green), "{}\n", successCount);
    fmt::print("Failed: ");
    fmt::print(fg(fmt::color::red), "{}\n", failureCount);

    if (failureCount > 0) {
        fmt::print(fg(fmt::color::red), "Some tests failed. Check the test summary for details.\n");
    } else {
        fmt::print(fg(fmt::color::green), "All tests passed!\n");
    }
    fmt::print(fg(fmt::color::green), "=========================================================================\n");
    for (auto& test : tests) {
        if (!test.getCapturedStdout().empty()) {
            fmt::print("Captured stdout for test '{}':\n{}\n", test.getName(), test.getCapturedStdout());
        }
        if (!test.getCapturedStderr().empty()) {
            fmt::print("Captured stderr for test '{}':\n{}\n", test.getName(), test.getCapturedStderr());
        }
    }
    fmt::print(fg(fmt::color::blue), "===================================END===================================\n");
}

void TestSuite::addTest(const std::string& testName, const std::function<void()>& testFunc) {
    tests.push_back(Test(testName, testFunc));
}

void TestSuite::setTestSuiteName(const std::string& name) {
    testSuiteName = name;
}
