/*
 * ===========================================================================
 * build.h
 *
 * A collection of QoL functionalities for C.
 *
 * Created: 30 Sep 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#pragma once

#ifndef SHL_BUILD_H
#define SHL_BUILD_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

// TODO: log to system log? (like journalctl)
// TODO: BuildConfig should have a auto_rebuild
// TODO: command_flags and compiler_flags should be an array

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef SHL_ASSERT
    #include <assert.h>
    #define SHL_ASSERT assert
#endif /* SHL_ASSERT */

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
    #include <pthread.h>
    #include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <io.h>
#else
    #error Unsupported platform
#endif

// SHL_USE_LOGGER

    // Log levels
    typedef enum {
        SHL_LOG_DEBUG = 0,   // like a extended log
        SHL_LOG_INFO,        //
        SHL_LOG_HINT,        // a special hint, not something wrong
        SHL_LOG_WARN,        // something before error
        SHL_LOG_ERROR,       // well, the ship is sinking
        SHL_LOG_CRITICAL,    // fucked up but not breaking (e.g. memory leak)
        SHL_LOG_NONE
    } shl_log_level_t;

    // Initialize logger with minimum level
    void shl_init_logger(shl_log_level_t level, bool color, bool time);

    // Use SHL logger if available
    // #ifndef SHL_USE_LOGGER
    //     #define shl_debug(fmt, ...)    printf(fmt "\n", ##__VA_ARGS__)
    //     #define shl_info(fmt, ...)     printf(fmt "\n", ##__VA_ARGS__)
    //     #define shl_hint(fmt, ...)     printf(fmt "\n", ##__VA_ARGS__)
    //     #define shl_warn(fmt, ...)     printf(fmt "\n", ##__VA_ARGS__)
    //     #define shl_error(fmt, ...)    printf(fmt "\n", ##__VA_ARGS__)
    //     #define shl_critical(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
    // #else
        #define shl_debug(fmt, ...)    shl_log(SHL_LOG_DEBUG, fmt, ##__VA_ARGS__)
        #define shl_info(fmt, ...)     shl_log(SHL_LOG_INFO, fmt, ##__VA_ARGS__)
        #define shl_hint(fmt, ...)     shl_log(SHL_LOG_HINT, fmt, ##__VA_ARGS__)
        #define shl_warn(fmt, ...)     shl_log(SHL_LOG_WARN, fmt, ##__VA_ARGS__)
        #define shl_error(fmt, ...)    shl_log(SHL_LOG_ERROR, fmt, ##__VA_ARGS__)
        #define shl_critical(fmt, ...) shl_log(SHL_LOG_CRITICAL, fmt, ##__VA_ARGS__)
    // SHL_USE_LOGGER

// SHL_USE_LOGGER

// SHL_USE_CLI_PARSER

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

    void shl_init_argparser(int argc, char *argv[]);
    void shl_add_argument(const char *long_name, const char *default_val, const char *help_msg);
    shl_arg_t *shl_get_argument(const char *long_name);

// SHL_USE_CLI_PARSER

// SHL_USE_NO_BUILD

    #define MAX_TASKS 32

    typedef struct {
        char *source;
        char *output;
        char *compiler;       // cc, gcc, clang...
        char *compiler_flags; // array: -Wall, -O2, -std=c11 ...
        char *linker_flags;   // array: -lm, -pthread, ...
        char *include_dirs;   // array: -Iinclude, -Isrc/include ...
        char *libraries;      // array: -lm, -lssl, -lz ...
        char *library_dirs;   // array: -L/usr/local/lib ...
        char *defines;        // array: -DDEBUG, -DVERSION="1.0"
        bool autorun;         // nach Build automatisch ausführen
    } SHL_BuildConfig;

    typedef struct {
        const char* cmd;
        const char* cmd_flags;
    } SHL_SystemConfig;

    typedef struct {
        SHL_BuildConfig config;
        bool success;
    } SHL_BuildTask;

    static inline char* shl_default_compiler_flags(void);
    static inline SHL_BuildConfig shl_default_build_config(void);
    bool shl_build_project(SHL_BuildConfig* config);
    bool shl_system(SHL_SystemConfig* config);
    void shl_auto_rebuild(char *src);
    bool shl_dispatch_build(SHL_BuildConfig *config);
    char *shl_get_filename_no_ext(const char *path);
    void shl_wait_for_all_builds(void);
    bool shl_build_project_async(const SHL_BuildConfig* config);

// SHL_USE_NO_BUILD

// SHL_USE_FILE_OPS

    typedef struct {
        char **data;
        size_t len;
        size_t cap;
    } SHL_String;

    bool shl_mkdir(const char *path);
    bool shl_copy_file(const char *src_path, const char *dst_path);
    bool shl_copy_dir_rec(const char *src_path, const char *dst_path);
    bool shl_read_dir(const char *parent, const char *children);
    bool shl_read_file(const char *path, SHL_String* content);
    bool shl_write_file(const char *path, const void *data, size_t size);
    const char *shl_get_file_type(const char *path);
    bool shl_delete_file(const char *path);

// SHL_USE_FILE_OPS

// SHL_USE_DYN_ARRAY

    #define SHL_INIT_CAP 8

    // Reserve space for at least `n` elements
    #define shl_grow(vec, n)                                                                    \
        do {                                                                                    \
            if ((n) > (vec)->cap) {                                                             \
                size_t newcap = (vec)->cap ? (vec)->cap : SHL_INIT_CAP;                         \
                while (newcap < (n)) newcap *= 2;                                               \
                if ((vec)->cap == 0) {                                                          \
                    shl_log(SHL_LOG_DEBUG, "Dynamic array inits memory on %d.\n", newcap);                       \
                } else {                                                                        \
                    shl_log(SHL_LOG_DEBUG, "Dynamic array needs more memory (%d -> %d)!\n", (vec)->cap, newcap); \
                }                                                                               \
                void *tmp = realloc((vec)->data, newcap * sizeof(*(vec)->data));                \
                if (!tmp) {                                                                     \
                    shl_log(SHL_LOG_ERROR, "Dynamic array out of memory (need %zu elements)\n", n);              \
                    abort();                                                                    \
                }                                                                               \
                (vec)->data = tmp;                                                              \
                (vec)->cap = newcap;                                                            \
            }                                                                                   \
        } while (0)

    #define shl_shrink(vec)                                                                           \
        do {                                                                                          \
            if ((vec)->len < (vec)->cap / 2 && (vec)->cap > SHL_INIT_CAP) {                           \
                size_t newcap = (vec)->cap / 2;                                                       \
                shl_log(SHL_LOG_DEBUG, "Dynamic array can release some memory (%d -> %d)!\n", (vec)->cap, newcap); \
                void *tmp = realloc((vec)->data, newcap * sizeof(*(vec)->data));                      \
                if (tmp) {                                                                            \
                    (vec)->data = tmp;                                                                \
                    (vec)->cap = newcap;                                                              \
                }                                                                                     \
            }                                                                                         \
        } while (0)

    // Push a single item
    #define shl_push(vec, val)                 \
        do {                                   \
            shl_grow((vec), (vec)->len+1);     \
            (vec)->data[(vec)->len++] = (val); \
        } while (0)

    // Push multiple items at once
    #define shl_pushn(vec, src, count)                                             \
        do {                                                                       \
            size_t __cnt = (count);                                                \
            shl_grow((vec), (vec)->len + __cnt);                                   \
            memcpy((vec)->data + (vec)->len, (src), __cnt * sizeof(*(vec)->data)); \
            (vec)->len += __cnt;                                                   \
        } while (0)

    // Remove the last element
    #define shl_drop(vec)                             \
        do {                                          \
            if ((vec)->len == 0) {                    \
                shl_log(SHL_LOG_ERROR, "shl_drop() on empty array\n"); \
                abort();                              \
            }                                         \
            --(vec)->len;                             \
            shrink(vec);                              \
        } while (0)

    // Remove element at index n (shift elements down)
    #define shl_dropn(vec, n)                                         \
        do {                                                          \
            size_t __idx = (n);                                       \
            if (__idx >= (vec)->len) {                                \
                shl_log(SHL_LOG_ERROR, "shl_dropn(): index out of range\n");           \
                abort();                                              \
            }                                                         \
            memmove((vec)->data + __idx,                              \
                    (vec)->data + __idx + 1,                          \
                    ((vec)->len - __idx - 1) * sizeof(*(vec)->data)); \
            --(vec)->len;                                             \
            shrink(vec);                                              \
        } while (0)

    // Resize array to exactly n elements (uninitialized if grown)
    #define shl_resize(vec, n)    \
        do {                      \
            shl_grow((vec), (n)); \
            (vec)->len = (n);     \
        } while (0)

    // Free the buffer
    #define shl_release(vec)             \
        do {                             \
            free((vec)->data);           \
            (vec)->data = NULL;          \
            (vec)->len = (vec)->cap = 0; \
        } while (0)

    // Get last element (asserts non-empty)
    #define shl_back(vec) \
        ((vec)->len > 0 ? (vec)->data[(vec)->len-1] : (shl_log(SHL_LOG_ERROR, "shl_back() on empty array\n"), abort(), (vec)->data[0]))

    // Swap element i with last element (without removing)
    #define shl_swap(vec, i)                                   \
        do {                                                   \
            size_t __idx = (i);                                \
            if (__idx >= (vec)->len) {                         \
                shl_log(SHL_LOG_ERROR, "shl_swap(): out of range\n");           \
                abort();                                       \
            }                                                  \
            typeof((vec)->data[0]) __tmp = (vec)->data[__idx]; \
            (vec)->data[__idx] = (vec)->data[(vec)->len - 1];  \
            (vec)->data[(vec)->len - 1] = __tmp;               \
        } while (0)

    // Struct wrapper
    #define shl_list(T) \
        struct { T *data; size_t len, cap; }

// SHL_USE_DYN_ARRAY

// SHL_USE_HELPER
    #define SHL_UNUSED(value) (void)(value)
    #define SHL_TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
    #define SHL_UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

    #define SHL_ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))
    #define SHL_ARRAY_GET(array, index) \
        (SHL_ASSERT((size_t)(index) < SHL_ARRAY_LEN(array)), (array)[(size_t)(index)])
// SHL_USE_HELPER

#ifdef SHL_IMPLEMENTATION

    // SHL_USE_LOGGER

        // ANSI colors
        #define SHL_COLOR_RESET     "\x1b[0m"
        #define SHL_COLOR_DEBUG     "\x1b[90m" // gray
        #define SHL_COLOR_INFO      "\x1b[32m" // green
        #define SHL_COLOR_HINT      "\x1b[34m" // blue
        #define SHL_COLOR_WARN      "\x1b[33m" // yellow
        #define SHL_COLOR_ERROR     "\x1b[31m" // red
        #define SHL_COLOR_CRITICAL  "\x1b[35m" // purple

        static shl_log_level_t shl_logger_min_level = SHL_LOG_INFO;
        static bool shl_logger_color = true;
        static bool shl_logger_time = false;

        void shl_init_logger(shl_log_level_t level, bool color, bool time) {
            shl_logger_min_level = level;
            shl_logger_color = color;
            shl_logger_time = time;
        }

        static const char *shl_level_to_str(shl_log_level_t level) {
            switch (level) {
            case SHL_LOG_DEBUG:    return "DEBUG";
            case SHL_LOG_INFO:     return "INFO";
            case SHL_LOG_HINT:     return "HINT";
            case SHL_LOG_WARN:     return "WARN";
            case SHL_LOG_ERROR:    return "ERROR";
            case SHL_LOG_CRITICAL: return "CRITICAL";
            default:               return "UNK";
            }
        }

        static const char *shl_level_to_color(shl_log_level_t level) {
            switch (level) {
            case SHL_LOG_DEBUG:    return SHL_COLOR_DEBUG;
            case SHL_LOG_INFO:     return SHL_COLOR_INFO;
            case SHL_LOG_HINT:     return SHL_COLOR_HINT;
            case SHL_LOG_WARN:     return SHL_COLOR_WARN;
            case SHL_LOG_ERROR:    return SHL_COLOR_ERROR;
            case SHL_LOG_CRITICAL: return SHL_COLOR_CRITICAL;
            default:               return SHL_COLOR_RESET;
            }
        }

        void shl_log(shl_log_level_t level, const char *fmt, ...) {
            if (level < shl_logger_min_level || level >= SHL_LOG_NONE) return;

            const char *level_str = shl_level_to_str(level);

            const char *level_color = "";
            if (shl_logger_color) {
                level_color = shl_level_to_color(level);
            }

            if (shl_logger_time) {
                time_t t = time(NULL);
                struct tm *lt = localtime(&t);
                char buf[32];
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
                fprintf(stderr, "%s[%s]%s %s >>> ", level_color, level_str, SHL_COLOR_RESET, buf);
            } else {
                fprintf(stderr, "%s[%s]%s ", level_color, level_str, SHL_COLOR_RESET);
            }

            va_list args;
            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
        }

    // SHL_USE_LOGGER

    // SHL_USE_CLI_PARSER

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
                shl_log(SHL_LOG_ERROR, "Maximum number of arguments reached\n");
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

    // SHL_USE_CLI_PARSER

    // SHL_USE_NO_BUILD

        static void shl_ensure_dir_for_file(const char* filepath) {
            char dir[1024];
            strncpy(dir, filepath, sizeof(dir));
            dir[sizeof(dir)-1] = '\0';

            // Find the last slash/backslash
            char *slash = strrchr(dir, '/');
#if defined(_WIN32) || defined(_WIN64)
            if (!slash) slash = strrchr(dir, '\\');
#endif
            if (slash) {
                *slash = '\0';
                shl_mkdir(dir);
            }
        }

        static inline char* shl_default_compiler_flags(void) {
#if defined(_WIN32) || defined(_WIN64)
            return "";
#elif defined(__APPLE__) && defined(__MACH__)
            return "-Wall -Wextra";
#elif defined(__linux__)
            return "-Wall -Wextra";
#else
            return "";
#endif
        }

        static inline SHL_BuildConfig shl_default_build_config(void) {
            // Compiler and output defaults
#if defined(_WIN32) || defined(_WIN64)
            char *default_output = "a.exe";
            char *default_compiler = "gcc";
            char *default_compiler_flags = "";
#elif defined(__APPLE__) || defined(__MACH__)
            char *default_output = "a.out";
            char *default_compiler = "cc";
            char *default_compiler_flags = "";
#elif defined(__linux__)
            char *default_output = "a.out";
            char *default_compiler = "cc";
            char *default_compiler_flags = "";
#endif

            return (SHL_BuildConfig){.source = "",
                                   .output = default_output,
                                   .compiler = default_compiler,
                                   .compiler_flags = default_compiler_flags,
                                   .linker_flags = "",
                                   .include_dirs = "",
                                   .libraries = "",
                                   .library_dirs = "",
                                   .defines = "",
                                   .autorun = false};
    }

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
        static pthread_t task_threads[MAX_TASKS];
        static SHL_BuildTask task_data[MAX_TASKS];
        static int task_count = 0;

        static void* shl_build_thread(void* arg) {
            SHL_BuildTask* task = (SHL_BuildTask*)arg;
            task->success = shl_build_project(&task->config);

            if (task->success && task->config.autorun) {
                shl_log(SHL_LOG_INFO, "Auto-running %s\n", task->config.output);
                char cmd[1024];
                snprintf(cmd, sizeof(cmd), "./%s", task->config.output);
                int result = system(cmd);
                if (result != 0) {
                    shl_log(SHL_LOG_ERROR, "Program %s exited with code %d\n", task->config.output, result);
                }
            }
            return NULL;
        }

        bool shl_build_project_async(const SHL_BuildConfig* config) {
            if (task_count >= MAX_TASKS) {
                shl_log(SHL_LOG_ERROR, "Too many async build tasks (max %d)\n", MAX_TASKS);
                return false;
            }

            task_data[task_count].config = *config;
            task_data[task_count].success = false;
            if (pthread_create(&task_threads[task_count], NULL, shl_build_thread, &task_data[task_count]) != 0) {
                shl_log(SHL_LOG_ERROR, "Failed to create build thread.\n");
                return false;
            }

            task_count++;
            return true;
        }

        void shl_wait_for_all_builds(void) {
            for (int i = 0; i < task_count; i++) {
                pthread_join(task_threads[i], NULL);
                if (!task_data[i].success) {
                    shl_log(SHL_LOG_ERROR, "Build failed for %s\n", task_data[i].config.source);
                }
            }
            task_count = 0;
        }
#elif defined(_WIN32) || defined(_WIN64)
        static HANDLE task_threads[MAX_TASKS];
        static SHL_BuildTask task_data[MAX_TASKS];
        static int task_count = 0;

        static DWORD WINAPI shl_build_thread_win(LPVOID arg) {
            SHL_BuildTask* task = (SHL_BuildTask*)arg;
            task->success = shl_build_project(&task->config);

            if (task->success && task->config.autorun) {
                shl_log(SHL_LOG_INFO, "Auto-running %s\n", task->config.output);
                system(task->config.output);
            }
            return 0;
        }

        bool shl_build_project_async(const SHL_BuildConfig* config) {
            if (task_count >= MAX_TASKS) {
                shl_log(SHL_LOG_ERROR, "Too many async build tasks (max %d)\n", MAX_TASKS);
                return false;
            }

            task_data[task_count].config = *config;
            task_data[task_count].success = false;

            task_threads[task_count] = CreateThread(NULL, 0, shl_build_thread_win, &task_data[task_count], 0, NULL);
            if (!task_threads[task_count]) {
                shl_log(SHL_LOG_ERROR, "Failed to create build thread.\n");
                return false;
            }

            task_count++;
            return true;
        }

        void shl_wait_for_all_builds(void) {
            for (int i = 0; i < task_count; i++) {
                WaitForSingleObject(task_threads[i], INFINITE);
                CloseHandle(task_threads[i]);
                if (!task_data[i].success) {
                    shl_log(SHL_LOG_ERROR, "Build failed for %s\n", task_data[i].config.source);
                }
            }
            task_count = 0;
        }
#else
        #error Unsupported platform
#endif

        char *shl_get_filename_no_ext(const char *path) {
            // Find last '/' or '\\' for Windows paths
            const char *slash = strrchr(path, '/');
            const char *backslash = strrchr(path, '\\');
            const char *base = path;

            if (slash || backslash) {
                if (slash && backslash)
                base = (slash > backslash) ? slash + 1 : backslash + 1;
                else if (slash)
                base = slash + 1;
                else
                base = backslash + 1;
            }

            // Copy base name to buffer
            char *copy = strdup(base);
            if (!copy) return NULL;

            // Remove extension if any
            char *dot = strrchr(copy, '.');
            if (dot) *dot = '\0';

            return copy; // caller must free
        }

        void shl_auto_rebuild(char *src) {
            struct stat src_attr, out_attr;

#if defined(_WIN32) || defined(_WIN64)
            char *out = "build_new.exe";
#else
            char *out = shl_get_filename_no_ext(src);
#endif

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
                shl_debug("Rebuilding: %s -> %s\n", src, out);
#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
                SHL_BuildConfig own_build = shl_default_build_config();
                own_build.source = src;
                own_build.output = out;
                if (!shl_build_project(&own_build)) {
                    shl_log(SHL_LOG_ERROR, "Rebuild failed.\n");
                    exit(1);
                }

                // Restart the process with the new executable
                shl_debug("Restarting with updated build executable...\n");

                execv("./build", NULL);
                shl_log(SHL_LOG_ERROR, "Failed to restart build process.\n");
                exit(1);
#elif defined(_WIN32) || defined(_WIN64)
                // TODO: Not working on Windows

                const char *tmp_out = "build_new.exe";
                SHL_BuildConfig own_build = shl_default_build_config();
                own_build.source = src;
                own_build.output = out;
                if (!shl_build_project(&own_build)) {
                    shl_log(SHL_LOG_ERROR, "Rebuild failed.\n");
                    exit(1);
                }

                shl_debug("Restarting with updated build executable...\n");

                STARTUPINFO si = { sizeof(si) };
                PROCESS_INFORMATION pi;
                if (!CreateProcess(tmp_out, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    shl_log(SHL_LOG_ERROR, "Failed to restart build process.\n");
                    exit(1);
                }

                ExitProcess(0);
#else
                #error Unsupported platform
#endif

            } else {
                shl_debug("Up to date: %s\n", out);
            }
        }

        bool shl_dispatch_build(SHL_BuildConfig* config) {
            if (!config || !config->source || !config->output) {
                shl_log(SHL_LOG_ERROR, "Invalid build configuration %s, %s\n", config->source, config->output);
                return false;
            }

            shl_ensure_dir_for_file(config->output);

            // if (config->async) {
                //     shl_debug("Building in parallel!\n");
                //     return shl_build_project_async(config);
            // } else {
                //     shl_debug("Building in sequential!\n");
                //     return shl_build_project(config);
            // }

            bool res = shl_build_project(config);

            if (res && config->autorun) {
                const char *out = config->output;
                char command[256];
#if defined(__APPLE__) && defined(__MACH__) || defined(__linux__)
                snprintf(command, sizeof(command), "./%s", out);
#elif defined(_WIN32) || defined(_WIN64)
                static char fixed_output[256];
                snprintf(fixed_output, sizeof(fixed_output), "%s", config->output);
                for (char *p = fixed_output; *p; p++) {
                    if (*p == '/') *p = '\\';
                }
                snprintf(command, sizeof(command), ".\\%s", fixed_output);
#endif
                shl_log(SHL_LOG_INFO, "Auto Run the Executable %s\n", command);
                system(command);
            }

            return res;
        }

        bool shl_system(SHL_SystemConfig* config) {
            if (!config || !config->cmd || !config->cmd_flags) {
                shl_log(SHL_LOG_ERROR, "Invalid system configuration.\n");
                return false;
            }

            char command[1024];
            snprintf(command, sizeof(command), "%s %s",
                config->cmd,
                config->cmd_flags ? config->cmd_flags : ""
            );

            shl_log(SHL_LOG_INFO, "%s\n", command);
            int result = system(command);
            if (result != 0) {
                shl_log(SHL_LOG_ERROR, "Build failed with exit code %d.\n", result);
                return false;
            }

            shl_debug("Build succeeded");
            return true;

        }

        bool shl_build_project(SHL_BuildConfig* config) {
            if (!config || !config->source || !config->output) {
                shl_log(SHL_LOG_ERROR, "Invalid build configuration found during project build..\n");
                return false;
            }

            char command[1024];
            snprintf(command, sizeof(command), "%s %s %s -o %s %s",
#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
                config->compiler,
                config->compiler_flags ? config->compiler_flags : "",
                config->source,
                config->output,
                config->linker_flags ? config->linker_flags : ""
#elif defined(_WIN32) || defined(_WIN64)
                config->compiler,
                config->compiler_flags ? config->compiler_flags : "",
                config->source,
                config->output,
                config->linker_flags ? config->linker_flags : ""
#else
                #error Unsupported platform
#endif
            );

            shl_log(SHL_LOG_INFO, "%s\n", command);
            int result = system(command);
            if (result != 0) {
                shl_log(SHL_LOG_ERROR, "Build failed with exit code %d.\n", result);
                return false;
            }

            shl_log(SHL_LOG_DEBUG, "Build succeeded, output: %s\n", config->output);
            return true;
        }

    // SHL_USE_NO_BUILD

    // SHL_USE_FILE_OPS

        bool shl_mkdir(const char *path) {
#ifdef _WIN32
            int result = _mkdir(path);
#else
            int result = mkdir(path, 0755);
#endif
            SHL_UNUSED(result);
            // TODO: Error Handling of mkdir
            shl_log(SHL_LOG_DEBUG, "created directory `%s/`\n", path);
            return true;
        }

        bool shl_copy_file(const char *src_path, const char *dst_path) {
            SHL_UNUSED(src_path);
            SHL_UNUSED(dst_path);
            SHL_TODO("Not implemented yet");
            return false;
        }

        bool shl_copy_dir_rec(const char *src_path, const char *dst_path) {
            SHL_UNUSED(src_path);
            SHL_UNUSED(dst_path);
            SHL_TODO("Not implemented yet");
            return false;
        }

        bool shl_read_file(const char *path, SHL_String *content) {
            if (!path || !content) return false;

            FILE *fp = fopen(path, "r");
            if (!fp) return false;

            char *line = NULL;
            size_t n = 0;

            while (getline(&line, &n, fp) != -1) {
                size_t len = strlen(line);

                if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';

                char *copy = strdup(line);
                if (!copy) {
                    fclose(fp);
                    free(line);
                    return false;
                }

                shl_push(content, copy);
            }

            free(line);
            fclose(fp);
            return true;
        }

        bool shl_read_dir(const char *parent, const char *children) {
            SHL_UNUSED(parent);
            SHL_UNUSED(children);
            SHL_TODO("Not implemented yet");
            return false;
        }

        bool shl_write_file(const char *path, const void *data, size_t size) {
            SHL_UNUSED(path);
            SHL_UNUSED(data);
            SHL_UNUSED(size);
            SHL_TODO("Not implemented yet");
            return false;
        }

        const char *shl_get_file_type(const char *path) {
            SHL_UNUSED(path);
            SHL_TODO("Not implemented yet");
            return path;
        }

        bool shl_delete_file(const char *path) {
            SHL_UNUSED(path);
            SHL_TODO("Not implemented yet");
            return false;
        }

    // SHL_USE_FILE_OPS

#endif // SHL_IMPLEMENTATION

#ifdef SHL_STRIP_PREFIX

    // SHL_USE_HELPER
        #define ASSERT SHL_ASSERT
        #define UNUSED SHL_UNUSED
        #define TODO SHL_TODO
        #define UNREACHABLE SHL_UNREACHABLE
        #define ARRAY_LEN SHL_ARRAY_LEN
        #define ARRAY_GET SHL_ARRAY_GET
    // SHL_USE_HELPER

    // SHL_USE_LOGGER
        #define init_logger shl_init_logger
        #define debug shl_debug
        #define info shl_info
        #define hint shl_hint
        #define warn shl_warn
        #define error shl_error
        #define critical shl_critical
        #define LOG_NONE SHL_LOG_NONE
        #define LOG_DEBUG SHL_LOG_DEBUG
        #define LOG_INFO SHL_LOG_INFO
        #define LOG_HINT SHL_LOG_HINT
        #define LOG_WARN SHL_LOG_WARN
        #define LOG_ERROR SHL_LOG_ERROR
        #define LOG_CRITICAL SHL_LOG_CRITICAL
    // SHL_USE_LOGGER

    // SHL_USE_CLI_PARSER
        #define init_argparser shl_init_argparser
        #define add_argument   shl_add_argument
        #define get_argument   shl_get_argument
        #define arg_t          shl_arg_t
    // SHL_USE_CLI_PARSER

    // SHL_USE_NO_BUILD
        #define BuildConfig              SHL_BuildConfig
        #define BuildTask                SHL_BuildTask
        #define SystemConfig             SHL_SystemConfig
        #define auto_rebuild             shl_auto_rebuild
        #define get_filename_no_ext      shl_get_filename_no_ext
        #define build_project            shl_build_project
        #define default_compiler_flags   shl_default_compiler_flags
        #define default_build_config     shl_default_build_config
        #define system                   shl_system
        #define build_project_async      shl_build_project_async
        #define wait_for_all_builds      shl_wait_for_all_builds
        #define dispatch_build           shl_dispatch_build
    // SHL_USE_NO_BUILD

    // SHL_USE_DYN_ARRAY
        #define grow         shl_grow
        #define shrink       shl_shrink
        #define push         shl_push
        #define pushn        shl_pushn
        #define drop         shl_drop
        #define dropn        shl_dropn
        #define resize       shl_resize
        #define release      shl_release
        #define back         shl_back
        #define swap         shl_swap
        #define list         shl_list
    // SHL_USE_DYN_ARRAY

    // SHL_USE_HELPER
        #define ASSERT              SHL_ASSERT
        #define UNUSED              SHL_UNUSED
        #define TODO                SHL_TODO
        #define UNREACHABLE         SHL_UNREACHABLE
        #define ARRAY_LEN           SHL_ARRAY_LEN
        #define ARRAY_GET           SHL_ARRAY_GET
    // SHL_USE_HELPER

    // SHL_USE_FILE_OPS
        #define String        SHL_String
        #define mkdir_if      shl_mkdir
        #define copy_file     shl_copy_file
        #define copy_dir_rec  shl_copy_dir_rec
        #define read_dir      shl_read_dir
        #define read_file     shl_read_file
        #define write_file    shl_write_file
        #define get_file_type shl_get_file_type
        #define delete_file   shl_delete_file
    // SHL_USE_FILE_OPS

#endif // SHL_STRIP_PREFIX

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHL_BUILD_H
