#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "./build.h"

int main(int argc, char** argv) {
    auto_rebuild_plus("build.c", argc, argv, "build.h");

    add_argument("--tests",    NULL, "Compile all testcases");
    add_argument("--examples", NULL, "Compile all examples");
    add_argument("--demo",     NULL, "Compile all demos");

    init_argparser(argc, argv);

    shl_arg_t *cli_unittests = shl_get_argument("--tests");
    shl_arg_t *cli_examples = shl_get_argument("--examples");
    shl_arg_t *cli_demo = shl_get_argument("--demo");
    shl_arg_t *cli_clean = shl_get_argument("--clean");

    if (cli_unittests && cli_unittests->value) {
        info("tests\n");
        BuildConfig tests = default_build_config("tests/main.c", "out/unittests");
        if (!run(&tests)) return 1;
    } else if (cli_examples && cli_examples->value) {
        char *examples[][2] = {
            { "examples/000_shl_logger.c",          "out/000_logger" },
            { "examples/001_shl_cli_arg_parser.c",  "out/001_argparser" },
            { "examples/002_shl_dynamic_array.c",   "out/002_dyn_array" },
            { "examples/003_shl_helper.c",          "out/003_helper" },
            { "examples/004_shl_file_utils.c",      "out/004_file" },
            { "examples/005_shl_file_ops.c",        "out/005_file_ops" },
            { "examples/006_shl_hashmap.c",         "out/006_hashmap" },
            { "examples/007_shl_unittest.c",        "out/007_unittest" },
        };
        BuildConfig builds[ARRAY_LEN(examples)];
        for (size_t i = 0; i < ARRAY_LEN(examples); i++) {
            builds[i] = default_build_config(examples[i][0], examples[i][1]);
            if (!run(&builds[i])) return 1;
        }
    } else if (cli_demo && cli_demo->value) {
        char *demos[][2] = {
            { "examples/901_shl_demo_calculator.c", "out/901_demo_calculator" },
            { "examples/902_shl_demo_pointer.c",    "out/902_demo_pointer" },
            { "examples/903_shl_demo_lexer.c",      "out/903_demo_lexer" }
        };
        BuildConfig builds[ARRAY_LEN(demos)];
        for (size_t i = 0; i < ARRAY_LEN(demos); i++) {
            builds[i] = default_build_config(demos[i][0], demos[i][1]);
            if (!run(&builds[i])) return 1;
        }
    }

    return 0;
}
