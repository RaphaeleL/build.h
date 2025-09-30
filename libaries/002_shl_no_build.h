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
    const char* source;
    const char* output;
    const char* compiler;
    const char* compiler_flags;
    const char* linker_flags;
    bool cli;
} SHL_BuildConfig;

typedef struct {
    const char* command;
    const char* command_flags;
    bool cli;
} SHL_SystemConfig;

bool shl_build_project(const SHL_BuildConfig* config);
bool shl_run(const SHL_SystemConfig* config);
void shl_auto_rebuild();

// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
    #define BuildConfig       SHL_BuildConfig
    #define SystemConfig      SHL_SystemConfig
    #define auto_rebuild      shl_auto_rebuild
    #define build_project     shl_build_project
    #define run               shl_run
#endif // SHL_STRIP_PREFIX

#define SHL_USE_LOGGER
#include "000_shl_logger.h"

#ifdef SHL_IMPLEMENTATION
    #include <time.h>
    #include <sys/stat.h>
    #include <sys/types.h>

    void shl_auto_rebuild(void) {
        struct stat src_attr, out_attr;

        const char *src = "build.c";
        const char *out = "build";

        if (stat(src, &src_attr) != 0) {
            perror("stat source");
            return;
        }

        bool need_rebuild = false;
        if (stat(out, &out_attr) != 0) {
            need_rebuild = true;
        } else if (difftime(src_attr.st_mtime, out_attr.st_mtime) > 0) {
            need_rebuild = true;
        }

        if (need_rebuild) {
            info("Rebuilding: %s -> %s", src, out);
            BuildConfig own_build = {
                .source = "build.c",
                .output = "build",
                .compiler = "gcc"
            };
            if (!build_project(&own_build)) {
                error("Rebuild failed.");
            }
        } else {
            info("Up to date: %s", out);
        }
    }


    bool shl_run(const SHL_SystemConfig* config) {
        if (!config || !config->command || !config->command_flags) {
            error("Invalid system configuration.");
            return false;
        }

        char command[1024];
        snprintf(command, sizeof(command), "%s %s",
        config->command,
        config->command_flags ? config->command_flags : "");

        info("Executing system command: %s", command);
        int result = system(command);
        if (result != 0) {
            error("Build failed with exit code %d.", result);
            return false;
        }

        debug("Build succeeded");
        return true;

    }

    bool shl_build_project(const SHL_BuildConfig* config) {
        if (!config || !config->source || !config->output || !config->compiler) {
            error("Invalid build configuration.");
            return false;
        }

        char command[1024];
        snprintf(command, sizeof(command), "%s %s %s -o %s %s",
        config->compiler,
        config->compiler_flags ? config->compiler_flags : "",
        config->source,
        config->output,
        config->linker_flags ? config->linker_flags : "");

        info("Executing build command: %s", command);
        int result = system(command);
        if (result != 0) {
            error("Build failed with exit code %d.", result);
            return false;
        }

        debug("Build succeeded, output: %s", config->output);
        return true;
    }

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // SHL_NO_BUILD_H
