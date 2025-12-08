#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_dynarray_create) {
    qol_list(int) vector = {0};
    QOL_TEST_EQ(vector.len, 0, "vector length");
    QOL_TEST_EQ(vector.cap, 0, "vector capacity");
    release(&vector);
}

QOL_TEST(test_dynarray_push) {
    qol_list(int) vector = {0};
    for (int i = 0; i < 5; i++) {
        push(&vector, i);
    }
    QOL_TEST_EQ(vector.len, 5, "vector length");
    QOL_TEST_EQ(vector.cap, 8, "vector capacity");
    QOL_TEST_EQ(back(&vector), 4, "last element");
    release(&vector);
}

QOL_TEST(test_dynarray_push_variadic) {
    qol_list(int) vector = {0};
    push(&vector, 42, 43, 44);
    QOL_TEST_EQ(vector.len, 3, "vector length");
    QOL_TEST_EQ(vector.cap, 8, "vector capacity");
    QOL_TEST_EQ(back(&vector), 44, "last element");
    release(&vector);
}

QOL_TEST(test_dynarray_drop) {
    qol_list(int) vector = {0};
    for (int i = 0; i < 5; i++) {
        push(&vector, i);
    }
    drop(&vector);
    QOL_TEST_EQ(vector.len, 4, "vector length");
    QOL_TEST_EQ(vector.cap, 8, "vector capacity");
    QOL_TEST_EQ(back(&vector), 3, "last element");
    release(&vector);
}
