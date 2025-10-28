#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

#include "test_hashmap.h"
#include "test_dynarray.h"

int main() {
    // auto_rebuild_plus("main.c", "test_hashmap.h, test_dynarray.h");
    return test_run_all();
}
