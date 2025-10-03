#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_HELPER
#include "../build.h"

void pretty_print_array(int* xs, int len) {
    for (int i = 0; i < len; ++i) {
        printf("%d", xs[i]);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);
    printf("Hello, World!\n");

    int xs[] = {0, 1, 2, 3};
    pretty_print_array(xs, 4);
    pretty_print_array(xs, ARRAY_LEN(xs));

    TODO("Not implemented yet.");
    UNREACHABLE("Not implemented yet.");

    return 0;
}
