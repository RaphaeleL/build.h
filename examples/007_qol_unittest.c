#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_basic_arithmetic) {
    QOL_TEST_EQ(2 + 2, 4, "addition");
    QOL_TEST_EQ(10 - 5, 5, "subtraction");
    QOL_TEST_NEQ(1, 2, "inequality");
}

QOL_TEST(test_string_comparison) {
    QOL_TEST_STREQ("hello", "hello", "identical strings");
    QOL_TEST_STRNEQ("foo", "bar", "different strings");
}

QOL_TEST(test_truthiness) {
    QOL_TEST_TRUTHY(1, "non-zero is truthy");
    QOL_TEST_TRUTHY(true, "true is truthy");
    QOL_TEST_FALSY(0, "zero is falsy");
    QOL_TEST_FALSY(false, "false is falsy");
}

int main() {
    return test_run_all();
}

