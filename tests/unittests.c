#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

#include "test_hashmap.h"
#include "test_dynarray.h"
#include "test_file_ops.h"
#include "test_cli.h"
#include "test_helper.h"
#include "test_logger.h"
#include "test_build.h"


int main() {
    shl_auto_rebuild_plus("unittests.c", "test_hashmap.h", "test_dynarray.h", 
                          "test_file_ops.h", "test_cli.h", "test_helper.h", 
                          "test_logger.h", "test_build.h");
    return test_run_all();
}
