#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "./build.h"

Cmd cmd = {0};
Procs procs = {0};

int main() {
    auto_rebuild_plus(__FILE__, "build.h");

    char *examples[][2] = {
        {"examples/000_qol_logger.c", "out/000_logger"},
        {"examples/001_qol_cli_arg_parser.c", "out/001_argparser"},
        {"examples/002_qol_dynamic_array.c", "out/002_dyn_array"},
        {"examples/003_qol_helper.c", "out/003_helper"},
        {"examples/004_qol_file_utils.c", "out/004_file"},
        {"examples/005_qol_file_ops.c", "out/005_file_ops"},
        {"examples/006_qol_hashmap.c", "out/006_hashmap"},
        {"examples/007_qol_unittest.c", "out/007_unittest"},
        {"examples/008_qol_timer.c", "out/008_timer"},
        {"examples/010_qol_temp_alloc.c", "out/010_temp_alloc"},
        {"examples/011_qol_path_utils.c", "out/011_path_utils"},
        {"examples/901_qol_demo_calculator.c", "out/901_demo_calculator"},
        {"examples/902_qol_demo_pointer.c", "out/902_demo_pointer"},
        {"tests/unittests.c", "out/unittests"},
    };

    for (size_t i = 0; i < ARRAY_LEN(examples); i++) {
        cmd = default_c_build(examples[i][0], examples[i][1]);
        if (!run(&cmd, .procs = &procs)) return EXIT_SUCCESS;
    }

    if (!procs_wait(&procs)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
