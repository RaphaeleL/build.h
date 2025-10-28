#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"


SHL_TEST(test_dynarray_create) {
    shl_list(int) vector = {0};
    SHL_TEST_EQ(vector.len, 0, "vector length");
    SHL_TEST_EQ(vector.cap, 0, "vector capacity");
    release(&vector);
}

SHL_TEST(test_dynarray_push) {
    shl_list(int) vector = {0};
    for (int i = 0; i < 5; i++) {
        push(&vector, i);
    }
    SHL_TEST_EQ(vector.len, 5, "vector length");
    SHL_TEST_EQ(vector.cap, 8, "vector capacity");
    SHL_TEST_EQ(back(&vector), 4, "last element");
    release(&vector);
}

SHL_TEST(test_dynarray_pushn) {
    shl_list(int) vector = {0};
    int extra[] = {42, 43, 44};
    pushn(&vector, extra, 3);
    SHL_TEST_EQ(vector.len, 3, "vector length");
    SHL_TEST_EQ(vector.cap, 8, "vector capacity");
    SHL_TEST_EQ(back(&vector), 44, "last element");
    release(&vector);
}

SHL_TEST(test_dynarray_drop) {
    shl_list(int) vector = {0};
    for (int i = 0; i < 5; i++) {
        push(&vector, i);
    }
    drop(&vector);
    SHL_TEST_EQ(vector.len, 4, "vector length");
    SHL_TEST_EQ(vector.cap, 8, "vector capacity");
    SHL_TEST_EQ(back(&vector), 3, "last element");
    release(&vector);
}