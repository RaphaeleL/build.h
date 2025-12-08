#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_helper_array_macros) {
    int arr[] = {1, 2, 3, 4};
    QOL_TEST_EQ(QOL_ARRAY_LEN(arr), 4, "array length macro works");
    QOL_TEST_EQ(QOL_ARRAY_GET(arr, 2), 3, "array get macro works");
}

