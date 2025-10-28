#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

SHL_TEST(test_basic_arithmetic) {
    SHL_TEST_EQ(2 + 2, 4, "addition");
    SHL_TEST_EQ(10 - 5, 5, "subtraction");
    SHL_TEST_NEQ(1, 2, "inequality");
}

SHL_TEST(test_string_comparison) {
    SHL_TEST_STREQ("hello", "hello", "identical strings");
    SHL_TEST_STRNEQ("foo", "bar", "different strings");
}

SHL_TEST(test_truthiness) {
    SHL_TEST_TRUTHY(1, "non-zero is truthy");
    SHL_TEST_TRUTHY(true, "true is truthy");
    SHL_TEST_FALSY(0, "zero is falsy");
    SHL_TEST_FALSY(false, "false is falsy");
}

int main() {
    return test_run_all();
}

