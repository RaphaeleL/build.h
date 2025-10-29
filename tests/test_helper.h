#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

SHL_TEST(test_helper_array_macros) {
    int arr[] = {1, 2, 3, 4};
    SHL_TEST_EQ(SHL_ARRAY_LEN(arr), 4, "array length macro works");
    SHL_TEST_EQ(SHL_ARRAY_GET(arr, 2), 3, "array get macro works");
}

