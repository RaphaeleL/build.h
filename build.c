#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "./build.h"

int main() {
    auto_rebuild_plus(__FILE__, "build.h");

    char *examples[][2] = {
        { "examples/000_shl_logger.c",          "out/000_logger" },
        { "examples/001_shl_cli_arg_parser.c",  "out/001_argparser" },
        { "examples/002_shl_dynamic_array.c",   "out/002_dyn_array" },
        { "examples/003_shl_helper.c",          "out/003_helper" },
        { "examples/004_shl_file_utils.c",      "out/004_file" },
        { "examples/005_shl_file_ops.c",        "out/005_file_ops" },
        { "examples/006_shl_hashmap.c",         "out/006_hashmap" },
        { "examples/007_shl_unittest.c",        "out/007_unittest" },
        { "examples/008_shl_timer.c",           "out/008_timer" },
        { "examples/010_shl_temp_alloc.c",       "out/010_temp_alloc" },
        { "examples/011_shl_path_utils.c",      "out/011_path_utils" },
        { "examples/901_shl_demo_calculator.c", "out/901_demo_calculator" },
        { "examples/902_shl_demo_pointer.c",    "out/902_demo_pointer" },
        { "tests/unittests.c",                  "out/unittests" },
    };

    Cmd cmd = {0};
    Procs procs = {0};
    Timer timer = {0};

    timer_start(&timer);

    for (size_t i = 0; i < ARRAY_LEN(examples); i++) {
        cmd = default_c_build(examples[i][0], examples[i][1]);
        cmd.async = true;
        if (!run(&cmd, .procs = &procs)) {
            error("Failed to run build command for %s\n", examples[i][0]);
        }
    }

    if (!procs_wait(&procs)) {
        return EXIT_FAILURE;
    }

    double elapsed_ms = timer_elapsed_ms(&timer);
    info("Finished in %.3f ms\n", elapsed_ms);
    timer_reset(&timer);

    return EXIT_SUCCESS;
}
