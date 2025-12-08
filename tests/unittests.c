#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

#include "test_build.h"
#include "test_cli.h"
#include "test_cmd_exec.h"
#include "test_dynarray.h"
#include "test_file_ops.h"
#include "test_hashmap.h"
#include "test_helper.h"
#include "test_logger.h"
#include "test_timer.h"

int main() {
    return test_run_all();
}
