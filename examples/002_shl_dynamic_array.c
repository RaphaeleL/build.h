#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

typedef shl_list(int) Vector;

void print_da(Vector *vector) {
    info("len = %2zu, cap = %2zu, last = %2d -> ", vector->len, vector->cap, back(vector));
    for (size_t i = 0; i < vector->len; i++) {
        printf("%d ", vector->data[i]);
    }
    printf("\n");
}

int main(void) {
    Vector vector = {0};

    for (int i = 0; i < 5; i++) {
        push(&vector, i);
    }

    print_da(&vector);

    swap(&vector, 3);
    print_da(&vector);

    push(&vector, 10);
    push(&vector, 11);
    push(&vector, 12);

    print_da(&vector);

    push(&vector, 42, 43, 44);
    print_da(&vector);

    push(&vector, 2, 3, 4, 5, 6, 7, 8);
    print_da(&vector);

    drop(&vector);
    drop(&vector);
    drop(&vector);
    print_da(&vector);

    dropn(&vector, 0);
    print_da(&vector);

    release(&vector);

    return 0;
}
