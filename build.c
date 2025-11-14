#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "./build.h"


Cmd cmd = {0};

int main() {
    auto_rebuild_plus("build.c", "build.h");

    char *examples[][2] = {
        { "examples/000_shl_logger.c",          "out/000_logger" },
        { "examples/001_shl_cli_arg_parser.c",  "out/001_argparser" },
        { "examples/002_shl_dynamic_array.c",   "out/002_dyn_array" },
        { "examples/003_shl_helper.c",          "out/003_helper" },
        { "examples/004_shl_file_utils.c",      "out/004_file" },
        { "examples/005_shl_file_ops.c",        "out/005_file_ops" },
        { "examples/006_shl_hashmap.c",         "out/006_hashmap" },
        { "examples/007_shl_unittest.c",        "out/007_unittest" },
        { "examples/901_shl_demo_calculator.c", "out/901_demo_calculator" },
        { "examples/902_shl_demo_pointer.c",    "out/902_demo_pointer" },
        { "examples/903_shl_demo_lexer.c",      "out/903_demo_lexer" },
    };

    for (size_t i = 0; i < ARRAY_LEN(examples); i++) {
        Cmd cmd = default_build_config(examples[i][0], examples[i][1]);
        if (!run(&cmd)) { return 1; }
    }

    return 0;
}
