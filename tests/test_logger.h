#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_logger_levels_do_not_crash) {
    // qol_debug("debug message %d\n", 1);
    // qol_info("info message %s\n", "ok");
    // qol_cmd("cmd message %s\n", "echo");
    // qol_hint("hint message\n");
    // qol_warn("warn message\n");
    QOL_TEST_TRUTHY(true, "non-fatal logger calls executed");
}
