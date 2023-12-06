#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <fmt/color.h>

template <typename T>
std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename T>
void assertEquals(const T& expected, const T& actual) {
    if (expected != actual) {
        std::string expectedStr = toString(expected);
        std::string actualStr = toString(actual);
        std::ostringstream msg;
        msg << "Expected: " << expectedStr << ", but got: " << actualStr;
        throw std::runtime_error(msg.str());
    }
}

class Test {
public:
    using TestFunc = std::function<void()>;

    Test(const std::string& testName, TestFunc testFunc)
        : testName(testName), testFunc(testFunc) {}

    bool Run(const std::string& annotation);
    std::string getName();
    std::string getCapturedStderr();
    std::string getCapturedStdout();

private:
    std::string testName;
    TestFunc testFunc;
    std::string capturedStdout;
    std::string capturedStderr;
};

class TestSuite {
public:
    TestSuite();
    void runTests();
    void setTestSuiteName(const std::string& name);

protected:
    std::vector<Test> tests;
    std::string testSuiteName;

    void addTest(const std::string& testName, const std::function<void()>& testFunc);

private:
    int successCount;
    int failureCount;
};

#endif // TEST_H
