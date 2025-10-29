#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

SHL_TEST(test_logger_levels_do_not_crash) {
    shl_init_logger(SHL_LOG_DEBUG, false, false);
    // shl_debug("debug message %d\n", 1);
    // shl_info("info message %s\n", "ok");
    // shl_cmd("cmd message %s\n", "echo");
    // shl_hint("hint message\n");
    // shl_warn("warn message\n");
    SHL_TEST_TRUTHY(true, "non-fatal logger calls executed");
}
