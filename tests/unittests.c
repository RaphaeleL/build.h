#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

#include "test_build.h"
#include "test_cli.h"
#include "test_dynarray.h"
#include "test_file_ops.h"
#include "test_hashmap.h"
#include "test_helper.h"
#include "test_logger.h"
#include "test_timer.h"

int main() {
    shl_auto_rebuild_plus(
        "tests/unittests.c", "tests/test_hashmap.h", "tests/test_dynarray.h",
        "tests/test_file_ops.h", "tests/test_cli.h", "tests/test_helper.h",
        "tests/test_logger.h", "tests/test_build.h", "tests/test_timer.h");
    return test_run_all();
}
