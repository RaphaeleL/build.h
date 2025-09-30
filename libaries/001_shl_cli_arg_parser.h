/*
 * 001_shl_cli_arg_parser.h
 *
 * SHL for a CLI Argument Parser
 *
 * Created at:  29. Sep 2025
 * Author:      Raphaele Salvatore Licciardo
 *
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 *
 */

#ifndef SHL_CLI_ARG_PARSER_H
#define SHL_CLI_ARG_PARSER_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SHL_ARG_MAX 128

typedef struct {
    const char *long_name;   // "--foo"
    char short_name;         // 'f'
    const char *default_val; // default value as string
    const char *help_msg;    // help message
    const char *value;       // actual value from argv
} shl_arg_t;

typedef struct {
    shl_arg_t args[SHL_ARG_MAX];
    int count;
} shl_argparser_t;

extern shl_argparser_t shl_parser;

// Initializes the CLI argument parser
void shl_init_argparser(int argc, char *argv[]);

// Define a CLI argument
void shl_add_argument(const char *long_name, const char *default_val, const char *help_msg);

// Returns pointer to argument struct by long name
shl_arg_t *shl_get_argument(const char *long_name);

// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
    #define init_argparser shl_init_argparser
    #define add_argument   shl_add_argument
    #define get_argument   shl_get_argument
    #define arg_t          shl_arg_t
#endif // SHL_STRIP_PREFIX

// Use SHL logger if available
#ifdef SHL_USE_LOGGER
    #include "000_shl_logger.h"
#else
    #define info(fmt, ...)     printf(fmt "\n", ##__VA_ARGS__)
    #define debug(fmt, ...)    printf(fmt "\n", ##__VA_ARGS__)
    #define warn(fmt, ...)     printf(fmt "\n", ##__VA_ARGS__)
    #define error(fmt, ...)    printf(fmt "\n", ##__VA_ARGS__)
    #define critical(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#endif // SHL_USE_LOGGER

#ifdef SHL_IMPLEMENTATION
    #define SHL_IMPLEMENTATION
    #define SHL_STRIP_PREFIX
    #include "000_shl_logger.h"

    void shl_init_argparser(int argc, char *argv[]) {
        shl_add_argument("--help", NULL, "Show this help message"); // no value expected

        for (int i = 1; i < argc; i++) {
            for (int j = 0; j < shl_parser.count; j++) {
                shl_arg_t *arg = &shl_parser.args[j];

                // Long option match
                if (strcmp(argv[i], arg->long_name) == 0) {
                    if (strcmp(arg->long_name, "--help") == 0) {
                        arg->value = "1"; // flag is set
                    } else if (i + 1 < argc) {
                        arg->value = argv[i + 1];
                        i++;
                    }
                }
                // Short option match
                else if (argv[i][0] == '-' && argv[i][1] == arg->short_name) {
                    if (arg->short_name == 'h') {
                        arg->value = "1"; // flag is set
                    } else if (i + 1 < argc) {
                        arg->value = argv[i + 1];
                        i++;
                    }
                }
            }
        }

        // Show help if requested
        shl_arg_t *help = shl_get_argument("--help");
        if (help && help->value) {
            printf("Usage:\n");
            for (int i = 0; i < shl_parser.count; i++) {
                shl_arg_t *arg = &shl_parser.args[i];
                printf("  %s, -%c: %s (default: %s)\n",
                arg->long_name,
                arg->short_name,
                arg->help_msg ? arg->help_msg : "",
                arg->default_val ? arg->default_val : "none");
            }
            exit(0);
        }
    }


    shl_argparser_t shl_parser = { .count = 0 };

    void shl_add_argument(const char *long_name, const char *default_val, const char *help_msg) {
        if (shl_parser.count >= SHL_ARG_MAX) {
            error("Maximum number of arguments reached");
            return;
        }
        shl_arg_t *arg = &shl_parser.args[shl_parser.count++];
        arg->long_name = long_name;
        arg->short_name = long_name[2]; // take first letter after '--'
        arg->default_val = default_val;
        arg->help_msg = help_msg;
        arg->value = default_val; // initial value
    }

    shl_arg_t *shl_get_argument(const char *long_name) {
        for (int i = 0; i < shl_parser.count; i++) {
            if (strcmp(shl_parser.args[i].long_name, long_name) == 0)
            return &shl_parser.args[i];
        }
        return NULL;
    }

    int shl_arg_as_int(shl_arg_t *arg) {
        if (!arg || !arg->value) return 0;
        return atoi(arg->value);
    }

    const char *shl_arg_as_string(shl_arg_t *arg) {
        if (!arg) return "";
        return arg->value ? arg->value : "";
    }

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // SHL_CLI_ARG_PARSER_H
