#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#include "../libaries/000_shl_logger.h"
#include "../libaries/003_shl_dynamic_array.h"

typedef shl_list(int) Vector;

void print_da(Vector *vector) {
    info("len = %2zu, cap = %2zu, last = %2d -> ", vector->len, vector->cap, back(vector));
    for (size_t i = 0; i < vector->len; i++) {
        printf("%d ", vector->data[i]);
    }
    printf("\n");
}

int main(void) {
    init_logger((LogConfig_t){.level = LOG_DEBUG, .color = false});

    Vector vector = {0};

    for (int i = 0; i < 5; i++) {
        push(&vector, i);
    }

    print_da(&vector);

    push(&vector, 10);
    push(&vector, 11);
    push(&vector, 12);

    print_da(&vector);

    int extra[] = {42, 43, 44};
    pushn(&vector, extra, 3);
    print_da(&vector);

    remove_swap(&vector, 2);
    print_da(&vector);

    int another_extra[] = {2, 3, 4, 5, 6, 7, 8};
    pushn(&vector, another_extra, 7);
    print_da(&vector);

    drop(&vector);
    print_da(&vector);

    dropn(&vector, 0);
    print_da(&vector);

    release(&vector);

    return 0;
}
