#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

SHL_TEST(test_cli_parser_basic_and_short_opts) {
    add_argument("--threads", "4", "number of threads");
    add_argument("--output", "a.out", "output file");

    char* argv1[] = { (char*)"prog", (char*)"--threads", (char*)"8", (char*)"-o", (char*)"bin/app", NULL };
    int argc1 = 5;
    init_argparser(argc1, argv1);

    arg_t* th = get_argument("--threads");
    arg_t* out = get_argument("--output");
    SHL_TEST_TRUTHY(th && out, "arguments exist");
    SHL_TEST_STREQ(th->value, "8", "long option parsed value");
    SHL_TEST_STREQ(out->value, "bin/app", "short option parsed value");
}

SHL_TEST(test_cli_parser_defaults_when_missing) {
    add_argument("--mode", "debug", "build mode");
    char* argv2[] = { (char*)"prog", NULL };
    int argc2 = 1;
    init_argparser(argc2, argv2);
    arg_t* mode = get_argument("--mode");
    SHL_TEST_TRUTHY(mode, "mode arg exists");
    SHL_TEST_STREQ(mode->value, "debug", "default preserved when not provided");
}

