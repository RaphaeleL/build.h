#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

#include "test_hashmap.h"
#include "test_dynarray.h"
#include "test_file_ops.h"
#include "test_cli.h"
#include "test_helper.h"
#include "test_logger.h"

int main() {
    // cc -o unittests unittests.c
    shl_auto_rebuild_plus("unittests.c", "test_hashmap.h", "test_dynarray.h", "test_file_ops.h", "test_cli.h", "test_helper.h", "test_logger.h");
    return test_run_all();
}
