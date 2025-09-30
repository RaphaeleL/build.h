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

#define MAX_TASKS 32

typedef struct {
    const char* source;
    const char* output;
    const char* compiler;
    const char* compiler_flags;
    const char* linker_flags;
    bool async;
    bool autorun;
} SHL_BuildConfig;

typedef struct {
    const char* command;
    const char* command_flags;
} SHL_SystemConfig;

typedef struct {
    SHL_BuildConfig config;
    bool success;
} SHL_BuildTask;

bool shl_build_project(const SHL_BuildConfig* config);
bool shl_run(const SHL_SystemConfig* config);
void shl_auto_rebuild();
bool shl_build_project_async(const SHL_BuildConfig* config);
void shl_wait_for_all_builds(void);
bool shl_dispatch_build(const SHL_BuildConfig* config);

// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
    #define BuildConfig         SHL_BuildConfig
    #define BuildTask           SHL_BuildTask
    #define SystemConfig        SHL_SystemConfig
    #define auto_rebuild        shl_auto_rebuild
    #define build_project       shl_build_project
    #define run                 shl_run
    #define build_project_async shl_build_project_async
    #define wait_for_all_builds shl_wait_for_all_builds
    #define dispatch_build      shl_dispatch_build
#endif // SHL_STRIP_PREFIX

#define SHL_USE_LOGGER
#include "000_shl_logger.h"

#ifdef SHL_IMPLEMENTATION
    #include <time.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <pthread.h>
    #include <ctype.h>

    static pthread_t task_threads[MAX_TASKS];
    static SHL_BuildTask task_data[MAX_TASKS];
    static int task_count = 0;

    static void* shl_build_thread(void* arg) {
        SHL_BuildTask* task = (SHL_BuildTask*)arg;
        task->success = shl_build_project(&task->config);

        if (task->success && task->config.autorun) {
            info("Auto-running %s", task->config.output);
            char cmd[1024];
            snprintf(cmd, sizeof(cmd), "./%s", task->config.output);
            int result = system(cmd);
            if (result != 0) {
                error("Program %s exited with code %d", task->config.output, result);
            }
        }
        return NULL;
    }

    bool shl_build_project_async(const SHL_BuildConfig* config) {
        if (task_count >= MAX_TASKS) {
            error("Too many async build tasks (max %d)", MAX_TASKS);
            return false;
        }

        task_data[task_count].config = *config;
        task_data[task_count].success = false;

        if (pthread_create(&task_threads[task_count], NULL, shl_build_thread, &task_data[task_count]) != 0) {
            error("Failed to create build thread.");
            return false;
        }

        task_count++;
        return true;
    }

    void shl_wait_for_all_builds(void) {
        for (int i = 0; i < task_count; i++) {
            pthread_join(task_threads[i], NULL);
            if (!task_data[i].success) {
                error("Build failed for %s", task_data[i].config.source);
            }
        }
        task_count = 0;
    }

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

    bool shl_dispatch_build(const SHL_BuildConfig* config) {
        if (config->async) {
            debug("Building in parallel!");
            return shl_build_project_async(config);
        } else {
            debug("Building in sequential!");
            return shl_build_project(config);
        }
    }

    static char *shl_trim(char *s) {
        char *end;
        while (isspace((unsigned char)*s)) s++;
        if (*s == 0) return s;
        end = s + strlen(s) - 1;
        while (end > s && isspace((unsigned char)*end)) end--;
        end[1] = '\0';
        return s;
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
