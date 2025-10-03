#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#define SHL_USE_NO_BUILD
#define SHL_USE_HELPER
#include "./build.h"

int main() {
    init_logger((LogConfig_t){.level = LOG_INFO, .color = false});
    auto_rebuild();

    const char *examples[][2] = {
        { "examples/000_shl_logger.c",          "out/000_logger" },
        { "examples/001_shl_cli_arg_parser.c",  "out/001_argparser" },
        { "examples/002_shl_dynamic_array.c",   "out/002_dyn_array" },
        { "examples/003_shl_helper.c",          "out/003_helper" },
        { "examples/901_shl_demo_calculator.c", "out/901_demo_calculator" },
        { "examples/902_shl_demo_pointer.c",    "out/902_demo_pointer" }
    };

    BuildConfig builds[ARRAY_LEN(examples)];

    for (size_t i = 0; i < ARRAY_LEN(examples); i++) {
        builds[i] = (BuildConfig){
            .source = examples[i][0], .output = examples[i][1],
            .autorun = !true
        };
        if (!dispatch_build(&builds[i])) return 1;
    }

    return 0;
}
