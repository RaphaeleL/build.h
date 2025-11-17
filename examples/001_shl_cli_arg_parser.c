/*
 * ===========================================================================
 * 001_shl_cli_arg_parser.c
 *
 * Example usage for SHL CLI Argument Parser
 *
 * Created: 29 Sep 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#include <stdio.h>

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

int main(int argc, char *argv[]) {
    add_argument("--foo", "bar", "The foo argument");
    add_argument("--number", "42", "The number argument");

    init_argparser(argc, argv);

    shl_arg_t *arg1 = shl_get_argument("--number");
    if (arg1 && shl_arg_as_int(arg1) == 5) {
        info("Number is 5!\n");
    } else {
        info("Number is not 5, it is %d\n", shl_arg_as_int(arg1));
    }

    shl_arg_t *arg2 = shl_get_argument("--foo");
    if (arg2) {
        info("Foo is %s\n", arg2->value);
    }

    return EXIT_SUCCESS;
}
