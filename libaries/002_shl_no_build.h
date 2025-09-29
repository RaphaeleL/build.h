/*
 * 002_shl_no_build.h
 *
 * SHL for a build tool that does not require any build system.
 *
 * Created at:  29. Sep 2025
 * Author:      Raphaele Salvatore Licciardo
 *
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 *
 */

#ifndef SHL_NO_BUILD_H
#define SHL_NO_BUILD_H 

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    const char* source_file;
    const char* output_file;
    const char* compiler;
    const char* compiler_flags;
    const char* linker_flags;
} SHL_BuildConfig;

bool shl_build_project(const SHL_BuildConfig* config);

// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
#define BuildConfig   SHL_BuildConfig
#define build_project shl_build_project
#endif // SHL_STRIP_PREFIX

// Use SHL logger if available
#ifdef SHL_USE_LOGGER
#include "000_shl_logger.h"
#else
#define info(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define debug(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define warn(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define error(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define critical(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#endif // SHL_USE_LOGGER

#ifdef SHL_IMPLEMENTATION
#include <stdio.h>

bool shl_build_project(const SHL_BuildConfig* config) {
    if (!config || !config->source_file || !config->output_file || !config->compiler) {
        error("Invalid build configuration.");
        return false;
    }

    char command[1024];
    snprintf(command, sizeof(command), "%s %s %s -o %s %s",
             config->compiler,
             config->compiler_flags ? config->compiler_flags : "",
             config->source_file,
             config->output_file,
             config->linker_flags ? config->linker_flags : "");

    info("Executing build command: %s", command);
    int result = system(command);
    if (result != 0) {
        error("Build failed with exit code %d.", result);
        return false;
    }

    info("Build succeeded, output: %s", config->output_file);
    return true;
}

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHL_NO_BUILD_H
