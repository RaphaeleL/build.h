/* build.h - v0.0.1 - https://github.com/RaphaeleL/build.h
   ============================================================================
    File: build.h
    Description: Quality-of-life utilities and abstractions for C development.

    This header provides a collection of macros, inline utilities, build-time
    helpers and more, intended to simplify common C programming patterns,
    improve code clarity, and enhance portability across compilers and
    platforms.

    The Idea could be considered as a mix between nothings/stb and tsoding/nob.h

    ----------------------------------------------------------------------------
    Created : 02 Oct 2025
    Changed : 08 Dez 2025
    Author  : Raphaele Salvatore Licciardo
    License : MIT (see LICENSE for details)
    Version : 0.0.1
    ----------------------------------------------------------------------------

    Quick Example: Auto Rebuild the Build System

      // build.c
      #define QOL_IMPLEMENTATION
      #define QOL_STRIP_PREFIX
      #include "./build.h"

      int main() {
          auto_rebuild(__FILE__);

          Cmd build = default_c_build("demo.c", "demo");
          if (!run(&build)) {  // auto-releases on success or failure
              return EXIT_FAILURE;
          }
          // -> run `cc -o demo demo.c` on change of source file

          Cmd calc = default_c_build("calc.c", NULL);
          push(&calc, "-Wall", "-Wextra");
          if (!qol_run_always(&calc)) {  // auto-releases on success or failure
              return EXIT_FAILURE;
          }
          // -> run `cc -Wall -Wextra calc.c -o calc` always

          return EXIT_SUCCESS;
      }

    Further Example: Build System, Arg Parser, Helpers, Hashmap, Logger

      // demo.c
      #define QOL_IMPLEMENTATION
      #define QOL_STRIP_PREFIX
      #include "./build.h"

      int main(int argc, char** argv) {
          auto_rebuild_plus("demo.c", "build.h");

          const char* progr = shift(argc, argv);
          const char* param = shift(argc, argv);

          UNUSED(progr);

          if (strcmp(param, "hashmap")) {
              info("Hashmap Demo selected\n");
              HashMap* hm = hm_create();
              hm_put(hm, (void*)"prename", (void*)"john");
              hm_put(hm, (void*)"name", (void*)"doe");
              hm_put(hm, (void*)"age", (void*)30);
              hm_remove(hm, (void *)"prename");
              hm_release(hm);
          } else {
              info("No Demo selected\n");
          }
          return EXIT_SUCCESS;
      }

    ----------------------------------------------------------------------------

    Changelog:

      v0.0.1 (08.12.2025) - Initial release

    ----------------------------------------------------------------------------
    Copyright (c) 2025 Raphaele Salvatore Licciardo

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
   ============================================================================ */

#pragma once

#ifndef QOL_BUILD_H
#define QOL_BUILD_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>

#ifndef QOL_ASSERT
    #include <assert.h>
    #define QOL_ASSERT assert
#endif /* QOL_ASSERT */

// Normalize OS detection
#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define MACOS 1
#elif defined(__linux__)
    #define LINUX 1
#else
    #define UNKNOWN 1
#endif

#if defined(MACOS) || defined(LINUX)
    #include <pthread.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #ifndef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 199309L
    #endif
    #include <time.h>
#elif defined(WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <io.h>
    #include <direct.h>
    #include <shellapi.h>
#else
    #error Unsupported platform
#endif

//////////////////////////////////////////////////
/// ANSI COLORS //////////////////////////////////
//////////////////////////////////////////////////

#define QOL_RESET           "\x1b[0m"  // RESET
#define QOL_RESET_FG        "\x1b[39m"
#define QOL_RESET_BG        "\x1b[49m"

#define QOL_BOLD            "\x1b[1m"  // Text Attributes
#define QOL_DIM             "\x1b[2m"
#define QOL_ITALIC          "\x1b[3m"
#define QOL_UNDERLINE       "\x1b[4m"
#define QOL_INVERT          "\x1b[7m"
#define QOL_HIDE            "\x1b[8m"
#define QOL_STRIKE          "\x1b[9m"

#define QOL_FG_BLACK        "\x1b[30m"  // Foreground
#define QOL_FG_RED          "\x1b[31m"
#define QOL_FG_GREEN        "\x1b[32m"
#define QOL_FG_YELLOW       "\x1b[33m"
#define QOL_FG_BLUE         "\x1b[34m"
#define QOL_FG_MAGENTA      "\x1b[35m"
#define QOL_FG_CYAN         "\x1b[36m"
#define QOL_FG_WHITE        "\x1b[37m"

#define QOL_FG_BBLACK       "\x1b[90m"  // Bright Foreground
#define QOL_FG_BRED         "\x1b[91m"
#define QOL_FG_BGREEN       "\x1b[92m"
#define QOL_FG_BYELLOW      "\x1b[93m"
#define QOL_FG_BBLUE        "\x1b[94m"
#define QOL_FG_BMAGENTA     "\x1b[95m"
#define QOL_FG_BCYAN        "\x1b[96m"
#define QOL_FG_BWHITE       "\x1b[97m"

#define QOL_BG_BLACK        "\x1b[40m"  // Background
#define QOL_BG_RED          "\x1b[41m"
#define QOL_BG_GREEN        "\x1b[42m"
#define QOL_BG_YELLOW       "\x1b[43m"
#define QOL_BG_BLUE         "\x1b[44m"
#define QOL_BG_MAGENTA      "\x1b[45m"
#define QOL_BG_CYAN         "\x1b[46m"
#define QOL_BG_WHITE        "\x1b[47m"

#define QOL_BG_BBLACK       "\x1b[100m"  // Bright Background
#define QOL_BG_BRED         "\x1b[101m"
#define QOL_BG_BGREEN       "\x1b[102m"
#define QOL_BG_BYELLOW      "\x1b[103m"
#define QOL_BG_BBLUE        "\x1b[104m"
#define QOL_BG_BMAGENTA     "\x1b[105m"
#define QOL_BG_BCYAN        "\x1b[106m"
#define QOL_BG_BWHITE       "\x1b[107m"

#define QOL_FG256(n)        "\x1b[38;5;" #n "m" // 256-Color Support
#define QOL_BG256(n)        "\x1b[48;5;" #n "m"

#define _QOL_STR_HELPER(x) #x  // Truecolor (RGB Support)
#define _QOL_STR(x) _QOL_STR_HELPER(x)
#define QOL_FG_RGB(r,g,b)   "\x1b[38;2;" _QOL_STR(r) ";" _QOL_STR(g) ";" _QOL_STR(b) "m"
#define QOL_BG_RGB(r,g,b)   "\x1b[48;2;" _QOL_STR(r) ";" _QOL_STR(g) ";" _QOL_STR(b) "m"

void QOL_enable_ansi(void);

/* Use: QOL_FG256(196) for bright red, QOL_BG256(21) for deep blue */
/* Use: QOL_FG_RGB(255, 128, 0) */

//////////////////////////////////////////////////
/// LOGGER ///////////////////////////////////////
//////////////////////////////////////////////////

// Log levels
typedef enum {
    QOL_LOG_DEBUG = 0,   // like an extended log
    QOL_LOG_INFO,        //
    QOL_LOG_CMD,         // an executed command
    QOL_LOG_HINT,        // special hint, not something wrong
    QOL_LOG_WARN,        // something before error
    QOL_LOG_ERROR,       // well, the ship is sinking
    QOL_LOG_CRITICAL,    // fucked up as hard as possible
    QOL_LOG_NONE
} qol_log_level_t;

// Initialize logger, with some basic values
void qol_init_logger(qol_log_level_t level, bool color, bool time);

// If set, the logger will also log to the given file. the file itself can
// be written in a c like manner to specify the format
void qol_init_logger_logfile(const char *format, ...);

// Get current time as formatted string
const char *qol_get_time(void);

// Forward declaration for logging function
void qol_log(qol_log_level_t level, const char *fmt, ...);

// Macros to easify the usage of log, instead of log(level, fmt) we are offering
// are more intuitive way of logging level(fmt)
#define qol_debug(fmt, ...)    qol_log(QOL_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define qol_info(fmt, ...)     qol_log(QOL_LOG_INFO, fmt, ##__VA_ARGS__)
#define qol_cmd(fmt, ...)      qol_log(QOL_LOG_CMD, fmt, ##__VA_ARGS__)
#define qol_hint(fmt, ...)     qol_log(QOL_LOG_HINT, fmt, ##__VA_ARGS__)
#define qol_warn(fmt, ...)     qol_log(QOL_LOG_WARN, fmt, ##__VA_ARGS__)
#define qol_error(fmt, ...)    qol_log(QOL_LOG_ERROR, fmt, ##__VA_ARGS__)
#define qol_critical(fmt, ...) qol_log(QOL_LOG_CRITICAL, fmt, ##__VA_ARGS__)

// TIME macro - returns current time as formatted string
#define QOL_TIME qol_get_time()

//////////////////////////////////////////////////
/// CLI_PARSER ///////////////////////////////////
//////////////////////////////////////////////////

#define QOL_ARG_MAX 128

typedef struct {
    const char *long_name;   // "--foo"
    char short_name;         // 'f'
    const char *default_val; // default value as string
    const char *help_msg;    // help message
    const char *value;       // actual value from argv
} qol_arg_t;

typedef struct {
    qol_arg_t args[QOL_ARG_MAX];
    int count;
} qol_argparser_t;

extern qol_argparser_t qol_parser;

// initialize the cli argument parser
void qol_init_argparser(int argc, char *argv[]);
// create an cli argument
void qol_add_argument(const char *long_name, const char *default_val, const char *help_msg);
// check if an argument is set and return the struct with the data
qol_arg_t *qol_get_argument(const char *long_name);

// pops an element from the beginning of a sized array (tsoding/nob.h)
#define qol_shift(size, elements) (QOL_ASSERT((size) > 0), (size)--, *(elements)++)

//////////////////////////////////////////////////
/// NO_BUILD /////////////////////////////////////
//////////////////////////////////////////////////

#define MAX_TASKS 32

// Process handle type
#ifdef WINDOWS
    typedef HANDLE QOL_Proc;
    #define QOL_INVALID_PROC INVALID_HANDLE_VALUE
#else
    typedef int QOL_Proc;
    #define QOL_INVALID_PROC (-1)
#endif

// Helper macro to check if a process handle is valid (for backward compatibility with boolean checks)
#define QOL_PROC_IS_VALID(proc) ((proc) != QOL_INVALID_PROC)

// Dynamic array of process handles
typedef struct {
    QOL_Proc *data;
    size_t len;
    size_t cap;
} QOL_Procs;

typedef struct {
    const char **data;
    size_t len;
    size_t cap;
    bool async;  // If true, run asynchronously; if false (default), run synchronously
} QOL_Cmd;

// Options for run/run_always functions
typedef struct {
    QOL_Procs *procs;  // If provided and async=true, process handle is added here
} QOL_RunOptions;

typedef struct {
    QOL_Cmd config;
    bool success;
} QOL_CmdTask;

// get the default compiler flags depending on the plattform
static inline char *qol_default_compiler_flags(void);
// Build a default build command (cc source -o output)
QOL_Cmd qol_default_c_build(const char *source, const char *output);

// Runs a BuildConfig based on the Timestamp
// Usage: run(&cmd) or run(&cmd, (RunOptions){ .procs = &procs })
// If config->async is true and opts.procs is provided, process handle is added to procs array
// If config->async is false (default), waits for completion and returns success/failure
bool qol_run_impl(QOL_Cmd *config, QOL_RunOptions opts);
// Always runs a BuildConfig
// Usage: run_always(&cmd) or run_always(&cmd, (RunOptions){ .procs = &procs })
// If config->async is true and opts.procs is provided, process handle is added to procs array
// If config->async is false (default), waits for completion and returns success/failure
bool qol_run_always_impl(QOL_Cmd* config, QOL_RunOptions opts);

// Macros to make options parameter optional with designated initializer syntax
// Usage: run(&cmd) or run(&cmd, .procs=&procs)
#define qol_run(cmd, ...) qol_run_impl(cmd, (QOL_RunOptions){__VA_ARGS__})
#define qol_run_always(cmd, ...) qol_run_always_impl(cmd, (QOL_RunOptions){__VA_ARGS__})
// Wait for an async process to complete
bool qol_proc_wait(QOL_Proc proc);
// Wait for all processes in a Procs array to complete
bool qol_procs_wait(QOL_Procs *procs);
// Auto Rebuild a Source File depending on the Timestamp
void qol_auto_rebuild(const char *src);
// Auto Rebuild a Source File and it's deps depending on the Timestamp
void qol_auto_rebuild_plus_impl(const char *src, ...);
// // Fetch any File through a URL and safe it into Name
// int qol_curl_file(const char *url, const char *name);

// Macro to automatically append NULL to variadic args
#define qol_auto_rebuild_plus(src, ...) qol_auto_rebuild_plus_impl(src, __VA_ARGS__, NULL)
// get the filename without it's extension, can be used to auto gen the output
// of a source file. TODO: Should be in @FILE_OPS, not in @NO_BUILD
char *qol_get_filename_no_ext(const char *path);

//////////////////////////////////////////////////
/// FILE_OPS /////////////////////////////////////
//////////////////////////////////////////////////

typedef struct {
    char **data;
    size_t len;
    size_t cap;
} QOL_String;

bool qol_mkdir(const char *path);
bool qol_mkdir_if_not_exists(const char *path);
bool qol_copy_file(const char *src_path, const char *dst_path);
bool qol_copy_dir_rec(const char *src_path, const char *dst_path);
bool qol_read_dir(const char *parent, const char *children);
bool qol_read_file(const char *path, QOL_String* content);
bool qol_write_file(const char *path, const void *data, size_t size);
const char *qol_get_file_type(const char *path);
bool qol_delete_file(const char *path);
bool qol_delete_dir(const char *path);
void qol_release_string(QOL_String* content);

// Path utilities
const char *qol_path_name(const char *path);
bool qol_rename(const char *old_path, const char *new_path);
const char *qol_get_current_dir_temp(void);
bool qol_set_current_dir(const char *path);
int qol_file_exists(const char *file_path);

// Rebuild detection
int qol_needs_rebuild(const char *output_path, const char **input_paths, size_t input_paths_count);
int qol_needs_rebuild1(const char *output_path, const char *input_path);

//////////////////////////////////////////////////
/// TEMP_ALLOCATOR ///////////////////////////////
//////////////////////////////////////////////////

#ifndef QOL_TEMP_CAPACITY
    #define QOL_TEMP_CAPACITY (8*1024*1024)
#endif

char *qol_temp_strdup(const char *cstr);
void *qol_temp_alloc(size_t size);
char *qol_temp_sprintf(const char *format, ...);
void qol_temp_reset(void);
size_t qol_temp_save(void);
void qol_temp_rewind(size_t checkpoint);

// Windows error handling
#ifdef WINDOWS
    char *qol_win32_error_message(DWORD err);
#endif

//////////////////////////////////////////////////
/// DYN_ARRAY ////////////////////////////////////
//////////////////////////////////////////////////

#define QOL_INIT_CAP 8

// Reserve space for at least `n` elements
#define qol_grow(vec, n)                                                                                     \
    do {                                                                                                     \
        if ((n) > (vec)->cap) {                                                                              \
            size_t newcap = (vec)->cap ? (vec)->cap : QOL_INIT_CAP;                                          \
            while (newcap < (n)) newcap *= 2;                                                                \
            if ((vec)->cap == 0) {                                                                           \
                qol_log(QOL_LOG_DEBUG, "Dynamic array inits memory on %d.\n", newcap);                       \
            } else {                                                                                         \
                qol_log(QOL_LOG_DEBUG, "Dynamic array needs more memory (%d -> %d)!\n", (vec)->cap, newcap); \
            }                                                                                                \
            void *tmp = realloc((vec)->data, newcap * sizeof(*(vec)->data));                                 \
            if (!tmp) {                                                                                      \
                qol_log(QOL_LOG_ERROR, "Dynamic array out of memory (need %zu elements)\n", n);              \
                abort();                                                                                     \
            }                                                                                                \
            (vec)->data = tmp;                                                                               \
            (vec)->cap = newcap;                                                                             \
        }                                                                                                    \
    } while (0)

#define qol_shrink(vec)                                                                                        \
    do {                                                                                                       \
        if ((vec)->len < (vec)->cap / 2 && (vec)->cap > QOL_INIT_CAP) {                                        \
            size_t newcap = (vec)->cap / 2;                                                                    \
            qol_log(QOL_LOG_DEBUG, "Dynamic array can release some memory (%d -> %d)!\n", (vec)->cap, newcap); \
            void *tmp = realloc((vec)->data, newcap * sizeof(*(vec)->data));                                   \
            if (tmp) {                                                                                         \
                (vec)->data = tmp;                                                                             \
                (vec)->cap = newcap;                                                                           \
            }                                                                                                  \
        }                                                                                                      \
    } while (0)

// Push a single item (internal implementation)
#define qol_push_impl(vec, val)            \
    do {                                   \
        qol_grow((vec), (vec)->len+1);     \
        (vec)->data[(vec)->len++] = (val); \
    } while (0)

// Variadic push: push(&vec, val) or push(&vec, a, b, c, ...) - truly dynamic, no limits
// Uses compound literal array trick - works with any number of arguments
// Note: Uses typeof (GCC/Clang extension) for type inference
#define qol_push(vec, ...) \
    do { \
        typeof(*vec) *__vec = (vec); \
        typeof(__vec->data[0]) __temp[] = {__VA_ARGS__}; \
        size_t __count = sizeof(__temp) / sizeof(__temp[0]); \
        for (size_t __i = 0; __i < __count; __i++) { \
            qol_push_impl(__vec, __temp[__i]); \
        } \
    } while (0)


// Remove the last element
#define qol_drop(vec)                                              \
    do {                                                           \
        if ((vec)->len == 0) {                                     \
            qol_log(QOL_LOG_ERROR, "qol_drop() on empty array\n"); \
            abort();                                               \
        }                                                          \
        --(vec)->len;                                              \
        qol_shrink(vec);                                           \
    } while (0)

// Remove element at index n (shift elements down)
#define qol_dropn(vec, n)                                                \
    do {                                                                 \
        size_t __idx = (n);                                              \
        if (__idx >= (vec)->len) {                                       \
            qol_log(QOL_LOG_ERROR, "qol_dropn(): index out of range\n"); \
            abort();                                                     \
        }                                                                \
        memmove((vec)->data + __idx,                                     \
                (vec)->data + __idx + 1,                                 \
                ((vec)->len - __idx - 1) * sizeof(*(vec)->data));        \
        --(vec)->len;                                                    \
        qol_shrink(vec);                                                 \
    } while (0)

// Resize array to exactly n elements (uninitialized if grown)
#define qol_resize(vec, n)    \
    do {                      \
        qol_grow((vec), (n)); \
        (vec)->len = (n);     \
    } while (0)

// Free the buffer
#define qol_release(vec)             \
    do {                             \
        free((vec)->data);           \
        (vec)->data = NULL;          \
        (vec)->len = (vec)->cap = 0; \
    } while (0)

// Get last element (asserts non-empty)
#define qol_back(vec) \
    ((vec)->len > 0 ? (vec)->data[(vec)->len-1] : \
     (fprintf(stderr, "[ERROR] qol_back() on empty array\n"), abort(), (vec)->data[0]))

// Swap element i with last element (without removing)
#define qol_swap(vec, i)                                          \
    do {                                                          \
        size_t __idx = (i);                                       \
        if (__idx >= (vec)->len) {                                \
            qol_log(QOL_LOG_ERROR, "qol_swap(): out of range\n"); \
            abort();                                              \
        }                                                         \
        typeof((vec)->data[0]) __tmp = (vec)->data[__idx];        \
        (vec)->data[__idx] = (vec)->data[(vec)->len - 1];         \
        (vec)->data[(vec)->len - 1] = __tmp;                      \
    } while (0)

// Struct wrapper
#define qol_list(T) \
    struct { T *data; size_t len, cap; }

//////////////////////////////////////////////////
/// HASHMAP //////////////////////////////////////
//////////////////////////////////////////////////

typedef enum {
    QOL_HM_EMPTY = 0,
    QOL_HM_USED,
    QOL_HM_DELETED
} qol_hm_entry_state_t;

typedef struct {
    void *key;
    void *value;
    size_t key_size;
    size_t value_size;
    qol_hm_entry_state_t state;
} QOL_HashMapEntry;

typedef struct {
    QOL_HashMapEntry *buckets;
    size_t capacity;
    size_t size;
} QOL_HashMap;

// Create an empty hashmap
QOL_HashMap *qol_hm_create();
// put a key/value pair into the hashmap
void qol_hm_put(QOL_HashMap *hm, void *key, void *value);
// get a value, based on the key
void *qol_hm_get(QOL_HashMap *hm, void *key);
// check if a hashmap contains a key
bool qol_hm_contains(QOL_HashMap *hm, void *key);
// remove a key/value pair, based on the key
bool qol_hm_remove(QOL_HashMap *hm, void *key);
// delete all the entries of an hashmap
void qol_hm_clear(QOL_HashMap* hm);
// free the hashmap memory
void qol_hm_release(QOL_HashMap* hm);
// get the size of a hashmap
size_t qol_hm_size(QOL_HashMap* hm);
// empty a hashmap
bool qol_hm_empty(QOL_HashMap* hm);

//////////////////////////////////////////////////
/// HELPER ///////////////////////////////////////
//////////////////////////////////////////////////

#define QOL_UNUSED(value) (void)(value)
#define QOL_TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define QOL_UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define QOL_ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))
#define QOL_ARRAY_GET(array, index) \
    (QOL_ASSERT((size_t)(index) < QOL_ARRAY_LEN(array)), (array)[(size_t)(index)])

//////////////////////////////////////////////////
/// UNITTEST /////////////////////////////////////
//////////////////////////////////////////////////

typedef struct {
    void (*func)(void);
    const char *name;
    const char *file;
    int line;
} qol_test_t;

void qol_test_register(const char *name, const char *file, int line, void (*test_func)(void));
void qol_test_fail(void);
int qol_test_run_all(void);
void qol_test_print_summary(void);

// Internal failure message storage
extern char qol_test_failure_msg[];

// Test macros
#define QOL_TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            snprintf(qol_test_failure_msg, sizeof(qol_test_failure_msg), "%s:%d: %s", __FILE__, __LINE__, message); \
            qol_test_fail(); \
            return; \
        } \
    } while(0)

#define QOL_TEST_EQ(a, b, message) QOL_TEST_ASSERT((a) == (b), message)
#define QOL_TEST_NEQ(a, b, message) QOL_TEST_ASSERT((a) != (b), message)
#define QOL_TEST_STREQ(a, b, message) QOL_TEST_ASSERT(strcmp((a), (b)) == 0, message)
#define QOL_TEST_STRNEQ(a, b, message) QOL_TEST_ASSERT(strcmp((a), (b)) != 0, message)
#define QOL_TEST_TRUTHY(value, message) QOL_TEST_ASSERT(value, message)
#define QOL_TEST_FALSY(value, message) QOL_TEST_ASSERT(!(value), message)

#define QOL_TEST(name) \
    static void qol_test_##name(void); \
    __attribute__((constructor)) static void qol_test_register_##name(void) { \
        qol_test_register(#name, __FILE__, __LINE__, qol_test_##name); \
    } \
    static void qol_test_##name(void)

//////////////////////////////////////////////////
/// TIMER ////////////////////////////////////////
//////////////////////////////////////////////////

// High-resolution timer structure
typedef struct {
#if defined(WINDOWS)
    LARGE_INTEGER start;
    LARGE_INTEGER frequency;
#else
    struct timespec start;
#endif
} QOL_Timer;

// Start a timer
void qol_timer_start(QOL_Timer *timer);

// Get elapsed time in seconds (as double)
double qol_timer_elapsed(QOL_Timer *timer);

// Get elapsed time in milliseconds (as double)
double qol_timer_elapsed_ms(QOL_Timer *timer);

// Get elapsed time in microseconds (as double)
double qol_timer_elapsed_us(QOL_Timer *timer);

// Get elapsed time in nanoseconds (as uint64_t)
uint64_t qol_timer_elapsed_ns(QOL_Timer *timer);

// Reset timer (restart from now)
void qol_timer_reset(QOL_Timer *timer);

//////////////////////////////////////////////////
/// QOL_IMPLEMENATION ////////////////////////////
//////////////////////////////////////////////////

#ifdef QOL_IMPLEMENTATION

    //////////////////////////////////////////////////
    /// ANSI COLORS //////////////////////////////////
    //////////////////////////////////////////////////

    // https://github.com/mlabbe/ansicodes/blob/main/ansicodes.h#L305-L316
    void QOL_enable_ansi(void) {
#if defined(WINDOWS)
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hStdout, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        mode |= DISABLE_NEWLINE_AUTO_RETURN;
        SetConsoleMode(hStdout, mode);
#endif
    }

    //////////////////////////////////////////////////
    /// LOGGER ///////////////////////////////////////
    //////////////////////////////////////////////////

    #define QOL_COLOR_RESET     QOL_RESET
    #define QOL_COLOR_INFO      QOL_FG_BBLACK
    #define QOL_COLOR_CMD       QOL_FG_CYAN
    #define QOL_COLOR_DEBUG     QOL_FG_GREEN
    #define QOL_COLOR_HINT      QOL_FG_BLUE
    #define QOL_COLOR_WARN      QOL_FG_YELLOW
    #define QOL_COLOR_ERROR     QOL_BOLD QOL_FG_RED
    #define QOL_COLOR_CRITICAL  QOL_BOLD QOL_FG_MAGENTA

    static qol_log_level_t qol_logger_min_level = QOL_LOG_INFO;
    static bool qol_logger_color = false;
    static bool qol_logger_time = true;
    static FILE *qol_log_file = NULL;

    void qol_init_logger(qol_log_level_t level, bool color, bool time) {
        qol_logger_min_level = level;
        qol_logger_color = color;
        qol_logger_time = time;
    }

    static char *qol_expand_path(const char *path) {
        if (!path) return NULL;

        // Check if path starts with ~
        if (path[0] == '~' && (path[1] == '/' || path[1] == '\0')) {
            const char *home = NULL;
#if defined(MACOS) || defined(LINUX)
            home = getenv("HOME");
#elif defined(WINDOWS)
            home = getenv("USERPROFILE");
            if (!home) home = getenv("HOMEPATH");
#endif
            if (!home) {
                fprintf(stderr, "Failed to get home directory\n");
                return strdup(path); // Return original path if home not found
            }

            // Allocate buffer: home + rest of path + null terminator
            size_t home_len = strlen(home);
            size_t path_len = strlen(path);
            char *expanded = (char *)malloc(home_len + path_len + 1);
            if (!expanded) return NULL;

            strcpy(expanded, home);
            if (path[1] == '/') {
                strcat(expanded, path + 1); // Skip ~ and keep /
            } else if (path[1] != '\0') {
                strcat(expanded, path + 1); // Skip ~
            }
            // If path is just "~", expanded is already home directory
            return expanded;
        }

        return strdup(path);
    }

    const char *qol_get_time(void) {
        static char time_buf[64];
        time_t t = time(NULL);
        struct tm *lt = localtime(&t);
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d_%H-%M-%S", lt);
        return time_buf;
    }

    void qol_init_logger_logfile(const char *format, ...) {
        // Close existing log file if open
        if (qol_log_file != NULL) {
            fclose(qol_log_file);
            qol_log_file = NULL;
        }

        // Open new log file if format is provided
        if (format != NULL) {
            char path[1024];

            va_list args;
            va_start(args, format);
            vsnprintf(path, sizeof(path), format, args);
            va_end(args);

            char *expanded_path = qol_expand_path(path);
            if (!expanded_path) {
                fprintf(stderr, "Failed to expand path: %s\n", path);
                return;
            }

            qol_log_file = fopen(expanded_path, "a"); // Append mode
            if (qol_log_file == NULL) {
                fprintf(stderr, "Failed to open log file: %s\n", expanded_path);
            }

            free(expanded_path);
        }
    }

    static const char *qol_level_to_str(qol_log_level_t level) {
        switch (level) {
        case QOL_LOG_DEBUG:    return "DEBUG";
        case QOL_LOG_INFO:     return "INFO";
        case QOL_LOG_CMD:      return "CMD";
        case QOL_LOG_HINT:     return "HINT";
        case QOL_LOG_WARN:     return "WARN";
        case QOL_LOG_ERROR:    return "ERROR";
        case QOL_LOG_CRITICAL: return "CRITICAL";
        default:               return "UNKNOWN";
        }
    }

    static const char *qol_level_to_color(qol_log_level_t level) {
        switch (level) {
        case QOL_LOG_DEBUG:    return QOL_COLOR_DEBUG;
        case QOL_LOG_INFO:     return QOL_COLOR_INFO;
        case QOL_LOG_CMD:      return QOL_COLOR_CMD;
        case QOL_LOG_HINT:     return QOL_COLOR_HINT;
        case QOL_LOG_WARN:     return QOL_COLOR_WARN;
        case QOL_LOG_ERROR:    return QOL_COLOR_ERROR;
        case QOL_LOG_CRITICAL: return QOL_COLOR_CRITICAL;
        default:               return QOL_COLOR_RESET;
        }
    }

    void qol_log(qol_log_level_t level, const char *fmt, ...) {
        if (level < qol_logger_min_level || level >= QOL_LOG_NONE) return;

        const char *level_str = qol_level_to_str(level);

        const char *level_color = "";
        if (qol_logger_color) {
            level_color = qol_level_to_color(level);
        }

        // TODO: decide if we want to reset color for certain log levels or not.
        // const char* reset_str = !(level == QOL_LOG_WARN || level == QOL_LOG_DEBUG) ? QOL_COLOR_RESET : "";

        char time_buf[32] = {0};
        if (qol_logger_time) {
            time_t t = time(NULL);
            struct tm *lt = localtime(&t);
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", lt);
            fprintf(stderr, "%s[%s]%s %s >>> %s", level_color, level_str, QOL_DIM, time_buf, QOL_COLOR_RESET);
        } else {
            fprintf(stderr, "%s[%s]%s ", level_color, level_str, QOL_COLOR_RESET);
        }

        // Write to log file (without color codes)
        if (qol_log_file != NULL) {
            if (qol_logger_time) {
                fprintf(qol_log_file, "[%s] %s >>> ", level_str, time_buf);
            } else {
                fprintf(qol_log_file, "[%s] ", level_str);
            }
        }

        va_list args;
        va_start(args, fmt);

        if (level == QOL_LOG_ERROR || level == QOL_LOG_CRITICAL) {
            fprintf(stderr, "\t\n");
            fprintf(stderr, "\t\n");
            fprintf(stderr, "\t              |    |    |                 \n");
            fprintf(stderr, "\t             )_)  )_)  )_)                %s: Leaving the Ship\n", level_str);
            fprintf(stderr, "\t            )___))___))___)               > ");

            vfprintf(stderr, fmt, args);

            fprintf(stderr, "\t           )____)____)_____)              \n");
            fprintf(stderr, "\t         _____|____|____|_____            \n");
            fprintf(stderr, "\t---------\\                   /---------  \n");
            fprintf(stderr, "\t  ^^^^^ ^^^^^^^^^^^^^^^^^^^^^             \n");
            fprintf(stderr, "\t    ^^^^      ^^^^     ^^^    ^^          \n");
            fprintf(stderr, "\t         ^^^^      ^^^                    \n");
            fprintf(stderr, "\t\n");

            // Write error message to file (plain text, no ASCII art)
            if (qol_log_file != NULL) {
                va_list args_copy;
                va_copy(args_copy, args);
                vfprintf(qol_log_file, fmt, args_copy);
                va_end(args_copy);
                fprintf(qol_log_file, "\n");
                fflush(qol_log_file);
            }
        } else {
            vfprintf(stderr, fmt, args);

            // Write message to file (plain text)
            if (qol_log_file != NULL) {
                va_list args_copy;
                va_copy(args_copy, args);
                vfprintf(qol_log_file, fmt, args_copy);
                va_end(args_copy);
                fprintf(qol_log_file, "\n");
                fflush(qol_log_file);
            }
        }

        va_end(args);

        if (level == QOL_LOG_ERROR) {
            fflush(NULL);
            exit(EXIT_FAILURE);
        } else if (level == QOL_LOG_CRITICAL) {
            fflush(NULL);
            abort();
        }
    }

    //////////////////////////////////////////////////1
    /// CLI_PARSER ///////////////////////////////////1
    //////////////////////////////////////////////////

    void qol_init_argparser(int argc, char *argv[]) {
        qol_add_argument("--help", NULL, "Show this help message"); // no value expected

        for (int i = 1; i < argc; i++) {
            for (int j = 0; j < qol_parser.count; j++) {
                qol_arg_t *arg = &qol_parser.args[j];

                // Long option match
                if (strcmp(argv[i], arg->long_name) == 0) {
                    if (strcmp(arg->long_name, "--help") == 0) {
                        arg->value = "1"; // flag is set
                    } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                        arg->value = argv[i + 1];
                        i++;
                    } else {
                        arg->value = "1"; // flag is set (no value provided)
                    }
                }
                // Short option match
                else if (argv[i][0] == '-' && argv[i][1] == arg->short_name) {
                    if (arg->short_name == 'h') {
                        arg->value = "1"; // flag is set
                    } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                        arg->value = argv[i + 1];
                        i++;
                    } else {
                        arg->value = "1"; // flag is set (no value provided)
                    }
                }
            }
        }

        // Show help if requested
        qol_arg_t *help = qol_get_argument("--help");
        if (help && help->value) {
            printf("Usage:\n");
            for (int i = 0; i < qol_parser.count; i++) {
                qol_arg_t *arg = &qol_parser.args[i];
                printf("  %s, -%c: %s (default: %s)\n",
                    arg->long_name,
                    arg->short_name,
                    arg->help_msg ? arg->help_msg : "",
                    arg->default_val ? arg->default_val : "none");
            }
            exit(0);
        }
    }

    qol_argparser_t qol_parser = { .count = 0 };

    void qol_add_argument(const char *long_name, const char *default_val, const char *help_msg) {
        if (qol_parser.count >= QOL_ARG_MAX) {
            qol_log(QOL_LOG_ERROR, "Maximum number of arguments reached\n");
            return;
        }
        qol_arg_t *arg = &qol_parser.args[qol_parser.count++];
        arg->long_name = long_name;
        arg->short_name = long_name[2]; // take first letter after '--'
        arg->default_val = default_val;
        arg->help_msg = help_msg;
        arg->value = default_val; // initial value
    }

    qol_arg_t *qol_get_argument(const char *long_name) {
        for (int i = 0; i < qol_parser.count; i++) {
            if (strcmp(qol_parser.args[i].long_name, long_name) == 0)
            return &qol_parser.args[i];
        }
        return NULL;
    }

    int qol_arg_as_int(qol_arg_t *arg) {
        if (!arg || !arg->value) return EXIT_SUCCESS;
        return atoi(arg->value);
    }

    const char *qol_arg_as_string(qol_arg_t *arg) {
        if (!arg) return "";
        return arg->value ? arg->value : "";
    }

    //////////////////////////////////////////////////
    /// NO_BUILD /////////////////////////////////////
    //////////////////////////////////////////////////

    static void qol_ensure_dir_for_file(const char* filepath) {
        char dir[1024];
        strncpy(dir, filepath, sizeof(dir));
        dir[sizeof(dir)-1] = '\0';

        // Find the last slash/backslash
        char *slash = strrchr(dir, '/');
#if defined(WINDOWS)
        if (!slash) slash = strrchr(dir, '\\');
#endif
        if (slash) {
            *slash = '\0';
            qol_mkdir_if_not_exists(dir);
        }
    }

    static inline char* qol_default_compiler_flags(void) {
#if defined(WINDOWS)
        return "";
#elif defined(__APPLE__) && defined(__MACH__)
        return "-Wall -Wextra";
#elif defined(__linux__)
        return "-Wall -Wextra";
#else
        return "";
#endif
    }

    QOL_Cmd qol_default_c_build(const char *source, const char *output) {
        QOL_Cmd cmd = {0};

#if defined(WINDOWS)
        qol_push(&cmd, "gcc");
#elif defined(__APPLE__) && defined(__MACH__)
        qol_push(&cmd, "cc");
#elif defined(__linux__)
        qol_push(&cmd, "cc");
#else
        qol_push(&cmd, "cc");
#endif

        // Push compiler flags as separate arguments
#if !defined(_WIN32) && !defined(_WIN64)
        qol_push(&cmd, "-Wall");
        qol_push(&cmd, "-Wextra");
#endif

        qol_push(&cmd, source);
        qol_push(&cmd, "-o");

        if (output) {
            qol_push(&cmd, output);
        } else {
            char *auto_output = qol_get_filename_no_ext(source);
            if (auto_output) {
                qol_push(&cmd, auto_output);
                free(auto_output);
            }
        }

        return cmd;
    }

    static bool qol_is_path1_modified_after_path2(const char *path1, const char *path2) {
        struct stat stat1, stat2;

        if (stat(path1, &stat1) != 0) return false;
        if (stat(path2, &stat2) != 0) return true;

        return difftime(stat1.st_mtime, stat2.st_mtime) > 0;
    }

    char *qol_get_filename_no_ext(const char *path) {
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

    // int qol_curl_file(const char *url, const char *name) {
    //     QOL_Cmd curl = {0};
    //     qol_push(&curl, "curl", "-sSL");
    //     qol_push(&curl, url);
    //     qol_push(&curl, "-o", name);
    //     if (!qol_run_always(&curl)) return EXIT_FAILURE;
    //     return EXIT_SUCCESS;
    // }

    void qol_auto_rebuild(const char *src) {
        if (!src) return;

        struct stat src_attr, out_attr;

#if defined(WINDOWS)
        char *out = "build_new.exe";
#else
        char *out = qol_get_filename_no_ext(src);
        if (!out) return;
#endif

        if (stat(src, &src_attr) != 0) {
            qol_log(QOL_LOG_ERROR, "No such file or directory (%s).\n", src);
#if !defined(_WIN32) && !defined(_WIN64)
            free(out);
#endif
            return;
        }

        bool need_rebuild = false;
        if (stat(out, &out_attr) != 0) {
            need_rebuild = true;
        } else if (difftime(src_attr.st_mtime, out_attr.st_mtime) > 0) {
            need_rebuild = true;
        }

        if (need_rebuild) {
            qol_debug("Rebuilding: %s -> %s\n", src, out);
#if defined(MACOS) || defined(LINUX)
            QOL_Cmd own_build = qol_default_c_build(src, out);
            if (!qol_run_always(&own_build)) {
                qol_release(&own_build);
                qol_log(QOL_LOG_ERROR, "Rebuild failed.\n");
#if !defined(_WIN32) && !defined(_WIN64)
                free(out);
#endif
                exit(1);
            }
            qol_release(&own_build);

            qol_debug("Restarting with updated build executable...\n");
            char *restart_argv[] = {out, NULL};
            execv(out, restart_argv);
            qol_log(QOL_LOG_ERROR, "Failed to restart build process.\n");
#if !defined(_WIN32) && !defined(_WIN64)
            free(out);
#endif
            exit(1);
#elif defined(WINDOWS)
            QOL_Cmd own_build = qol_default_c_build(src, out);
            if (!qol_run_always(&own_build)) {
                qol_release(&own_build);
                qol_log(QOL_LOG_ERROR, "Rebuild failed.\n");
                exit(1);
            }
            qol_release(&own_build);

            qol_debug("Restarting with updated build executable...\n");
            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            if (!CreateProcess(out, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                qol_log(QOL_LOG_ERROR, "Failed to restart build process.\n");
                exit(1);
            }
            ExitProcess(0);
#else
            #error Unsupported platform
#endif
        } else {
            qol_debug("Up to date: %s\n", out);
#if !defined(_WIN32) && !defined(_WIN64)
            free(out);
#endif
        }
    }

    // Auto-rebuild with additional dependency checking
    void qol_auto_rebuild_plus_impl(const char *src, ...) {
        if (!src) return;
        struct stat src_attr, out_attr;
#if defined(WINDOWS)
        const char *out = "build_new.exe";
#else
        char *out = qol_get_filename_no_ext(src);
#endif
        if (stat(src, &src_attr) != 0) {
            qol_log(QOL_LOG_ERROR, "No such file or directory (%s).\n", src);
#if !defined(_WIN32) && !defined(_WIN64)
            free(out);
#endif
            return;
        }

        bool need_rebuild = false;
        if (stat(out, &out_attr) != 0) {
            need_rebuild = true;
        } else if (difftime(src_attr.st_mtime, out_attr.st_mtime) > 0) {
            need_rebuild = true;
        }

        // Check additional dependencies from variadic arguments
        if (!need_rebuild) {
            va_list args;
            va_start(args, src);
            const char *dep_file = va_arg(args, const char*);
            while (dep_file != NULL) {
                if (qol_is_path1_modified_after_path2(dep_file, out)) {
                    qol_log(QOL_LOG_DEBUG, "Dependency %s is newer than binary, rebuild needed\n", dep_file);
                    need_rebuild = true;
                    // Don't break - continue checking all dependencies for logging
                }
                dep_file = va_arg(args, const char*);
            }
            va_end(args);
        }

        if (need_rebuild) {
            qol_debug("Rebuilding: %s -> %s\n", src, out);

// #ifdef QOL_UPDATE_ITSELF
//             const char* url = "https://raw.githubusercontent.com/RaphaeleL/build.h/refs/heads/main/build.h";
//             if (!qol_curl_file(url, "foo.h")) {
//                 qol_warn("Command 'curl' is not installed on this machine, to automatically refetch 'build.h' you need install it");
//             }
// #endif

#if defined(MACOS) || defined(LINUX)
            QOL_Cmd own_build = qol_default_c_build(src, out);
            if (!qol_run_always(&own_build)) {
                qol_release(&own_build);
                qol_log(QOL_LOG_ERROR, "Rebuild failed.\n");
#if !defined(_WIN32) && !defined(_WIN64)
                free(out);
#endif
                exit(1);
            }
            qol_release(&own_build);

            qol_debug("Restarting with updated build executable...\n");
            char *restart_argv[] = {out, NULL};
            execv(out, restart_argv);
            qol_log(QOL_LOG_ERROR, "Failed to restart build process.\n");
#if !defined(_WIN32) && !defined(_WIN64)
            free(out);
#endif
            exit(1);
#elif defined(WINDOWS)
            QOL_Cmd own_build = qol_default_c_build(src, out);
            if (!qol_run_always(&own_build)) {
                qol_release(&own_build);
                qol_log(QOL_LOG_ERROR, "Rebuild failed.\n");
                exit(1);
            }
            qol_release(&own_build);

            qol_debug("Restarting with updated build executable...\n");
            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            char cmdline[1024];
            snprintf(cmdline, sizeof(cmdline), "\"%s\"", out);
            if (!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                qol_log(QOL_LOG_ERROR, "Failed to restart build process.\n");
                exit(1);
            }
            ExitProcess(0);
#else
            #error Unsupported platform
#endif
        } else {
            qol_debug("Up to date: %s\n", out);
#if !defined(_WIN32) && !defined(_WIN64)
            free(out);
#endif
        }
    }

    // Helper: Extract source and output from command array
    // Assumes format: [compiler, flags..., source, "-o", output, ...]
    static const char* qol_cmd_get_source(QOL_Cmd* cmd) {
        if (!cmd || !cmd->data || cmd->len < 2) return NULL;

        // Find "-o" flag
        for (size_t i = 0; i < cmd->len - 1; i++) {
            if (cmd->data[i] && strcmp(cmd->data[i], "-o") == 0) {
                // Source should be before "-o"
                if (i > 0) {
                    // Skip compiler and flags, find first .c/.cpp file
                    for (size_t j = 1; j < i; j++) {
                        if (cmd->data[j] && strstr(cmd->data[j], ".c") != NULL) {
                            return cmd->data[j];
                        }
                    }
                    // Fallback: return item before "-o"
                    return cmd->data[i - 1];
                }
            }
        }

        // Fallback: find first .c file
        for (size_t i = 1; i < cmd->len; i++) {
            if (cmd->data[i] && strstr(cmd->data[i], ".c") != NULL) {
                return cmd->data[i];
            }
        }

        return NULL;
    }

    static const char* qol_cmd_get_output(QOL_Cmd* cmd) {
        if (!cmd || !cmd->data || cmd->len < 2) return NULL;

        // Find "-o" flag
        for (size_t i = 0; i < cmd->len - 1; i++) {
            if (cmd->data[i] && strcmp(cmd->data[i], "-o") == 0) {
                // Output is after "-o"
                return cmd->data[i + 1];
            }
        }

        return NULL;
    }

#ifdef WINDOWS
    char *qol_win32_error_message(DWORD err) {
        static char win32ErrMsg[4 * 1024] = {0};
        DWORD errMsgSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, LANG_USER_DEFAULT, win32ErrMsg,
                                          sizeof(win32ErrMsg), NULL);

        if (errMsgSize == 0) {
            if (GetLastError() != ERROR_MR_MID_NOT_FOUND) {
                if (sprintf(win32ErrMsg, "Could not get error message for 0x%lX", err) > 0) {
                    return (char *)&win32ErrMsg;
                } else {
                    return NULL;
                }
            } else {
                if (sprintf(win32ErrMsg, "Invalid Windows Error code (0x%lX)", err) > 0) {
                    return (char *)&win32ErrMsg;
                } else {
                    return NULL;
                }
            }
        }

        while (errMsgSize > 1 && isspace(win32ErrMsg[errMsgSize - 1])) {
            win32ErrMsg[--errMsgSize] = '\0';
        }

        return win32ErrMsg;
    }
#endif

    // Build command string for logging
    static void qol_cmd_log(QOL_Cmd* cmd) {
        if (!cmd || !cmd->data || cmd->len == 0) return;

        char command[4096] = {0};
        size_t pos = 0;
        for (size_t i = 0; i < cmd->len; i++) {
            if (!cmd->data[i]) continue;
            if (pos > 0 && pos < sizeof(command) - 1) {
                command[pos++] = ' ';
            }
            const char *item = cmd->data[i];
            size_t item_len = strlen(item);
            if (pos + item_len < sizeof(command) - 1) {
                strncpy(command + pos, item, sizeof(command) - pos - 1);
                pos += item_len;
            }
        }
        command[pos] = '\0';
        qol_log(QOL_LOG_CMD, "%s\n", command);
    }

    // Execute command array asynchronously - returns process handle
    static QOL_Proc qol_cmd_execute_async(QOL_Cmd* cmd) {
        if (!cmd || !cmd->data || cmd->len == 0) {
            qol_log(QOL_LOG_ERROR, "Invalid command: empty or null\n");
            return QOL_INVALID_PROC;
        }

        qol_cmd_log(cmd);

#ifdef WINDOWS
        // Build command line for Windows CreateProcess
        char cmdline[4096] = {0};
        size_t pos = 0;
        for (size_t i = 0; i < cmd->len; ++i) {
            if (i > 0 && pos < sizeof(cmdline) - 1) cmdline[pos++] = ' ';
            const char *arg = cmd->data[i];
            if (strchr(arg, ' ') || strchr(arg, '\t')) {
                if (pos < sizeof(cmdline) - 1) cmdline[pos++] = '"';
                size_t len = strlen(arg);
                if (pos + len < sizeof(cmdline) - 1) {
                    strncpy(cmdline + pos, arg, sizeof(cmdline) - pos - 1);
                    pos += len;
                }
                if (pos < sizeof(cmdline) - 1) cmdline[pos++] = '"';
            } else {
                size_t len = strlen(arg);
                if (pos + len < sizeof(cmdline) - 1) {
                    strncpy(cmdline + pos, arg, sizeof(cmdline) - pos - 1);
                    pos += len;
                }
            }
        }
        cmdline[pos] = '\0';

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        BOOL success = CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        if (!success) {
            qol_log(QOL_LOG_ERROR, "Could not create process: %s\n", qol_win32_error_message(GetLastError()));
            return QOL_INVALID_PROC;
        }

        CloseHandle(pi.hThread);
        return pi.hProcess;
#else
        pid_t pid = fork();
        if (pid < 0) {
            qol_log(QOL_LOG_ERROR, "Could not fork process: %s\n", strerror(errno));
            return QOL_INVALID_PROC;
        }

        if (pid == 0) {
            // Child process
            QOL_Cmd cmd_null = {0};
            for (size_t i = 0; i < cmd->len; i++) {
                qol_push(&cmd_null, cmd->data[i]);
            }
            qol_push(&cmd_null, NULL);

            if (execvp(cmd->data[0], (char * const*) cmd_null.data) < 0) {
                qol_log(QOL_LOG_ERROR, "Could not exec process: %s\n", strerror(errno));
                exit(1);
            }
            QOL_UNREACHABLE("qol_cmd_execute_async");
        }

        return pid;
#endif
    }

    // Wait for a process to complete
    bool qol_proc_wait(QOL_Proc proc) {
        if (proc == QOL_INVALID_PROC) return false;

#ifdef WINDOWS
        DWORD result = WaitForSingleObject(proc, INFINITE);
        if (result == WAIT_FAILED) {
            qol_log(QOL_LOG_ERROR, "Could not wait on child process: %s\n", qol_win32_error_message(GetLastError()));
            CloseHandle(proc);
            return false;
        }

        DWORD exit_code;
        if (!GetExitCodeProcess(proc, &exit_code)) {
            qol_log(QOL_LOG_ERROR, "Could not get process exit code: %s\n", qol_win32_error_message(GetLastError()));
            CloseHandle(proc);
            return false;
        }

        CloseHandle(proc);

        if (exit_code != 0) {
            qol_log(QOL_LOG_ERROR, "Command failed with exit code %lu\n", exit_code);
            return false;
        }

        return true;
#else
        int wstatus;
        if (waitpid(proc, &wstatus, 0) < 0) {
            qol_log(QOL_LOG_ERROR, "Could not wait for process: %s\n", strerror(errno));
            return false;
        }

        if (WIFEXITED(wstatus)) {
            int exit_code = WEXITSTATUS(wstatus);
            if (exit_code != 0) {
                qol_log(QOL_LOG_ERROR, "Command failed with exit code %d\n", exit_code);
                return false;
            }
        } else if (WIFSIGNALED(wstatus)) {
            qol_log(QOL_LOG_ERROR, "Command terminated by signal %d\n", WTERMSIG(wstatus));
            return false;
        }

        return true;
#endif
    }

    // Wait for all processes in a Procs array to complete
    bool qol_procs_wait(QOL_Procs *procs) {
        if (!procs) return false;

        bool all_success = true;
        for (size_t i = 0; i < procs->len; i++) {
            if (procs->data[i] != QOL_INVALID_PROC) {
                if (!qol_proc_wait(procs->data[i])) {
                    all_success = false;
                }
            }
        }
        // Clear the procs array after waiting
        procs->len = 0;
        return all_success;
    }

    bool qol_run_impl(QOL_Cmd* config, QOL_RunOptions opts) {
        if (!config || !config->data || config->len == 0) {
            qol_log(QOL_LOG_ERROR, "Invalid build configuration\n");
            if (config) qol_release(config);
            return false;
        }

        const char *source = qol_cmd_get_source(config);
        const char *output = qol_cmd_get_output(config);

        if (!source || !output) {
            qol_log(QOL_LOG_ERROR, "Could not extract source or output from command\n");
            qol_release(config);
            return false;
        }

        qol_ensure_dir_for_file(output);

        // Check if rebuild is needed
        if (!qol_is_path1_modified_after_path2(source, output)) {
            qol_log(QOL_LOG_DEBUG, "Up to date: %s\n", output);
            qol_release(config);
            return true; // Already up to date
        }

        return qol_run_always_impl(config, opts);
    }

    bool qol_run_always_impl(QOL_Cmd* config, QOL_RunOptions opts) {
        if (!config || !config->data || config->len == 0) {
            qol_log(QOL_LOG_ERROR, "Invalid build configuration\n");
            if (config) qol_release(config);
            return false;
        }

        QOL_Proc proc;
        if (opts.procs) {
            // Async mode: start process and add to procs array if provided
            proc = qol_cmd_execute_async(config);
            if (proc == QOL_INVALID_PROC) {
                qol_release(config);
                return false;
            }
            // Add proc to procs array if provided
            if (opts.procs) {
                qol_push(opts.procs, proc);
            }
            qol_release(config);  // Release command, proc is tracked in procs array
            return true;
        } else {
            // Sync mode: wait for completion (backward compatible behavior)
            proc = qol_cmd_execute_async(config);
            if (proc == QOL_INVALID_PROC) {
                qol_release(config);
                return false;
            }
            bool success = qol_proc_wait(proc);
            qol_release(config);
            return success;
        }
    }

    //////////////////////////////////////////////////
    /// TEMP_ALLOCATOR ///////////////////////////////
    //////////////////////////////////////////////////

    static size_t qol_temp_size = 0;
    static char qol_temp[QOL_TEMP_CAPACITY] = {0};

    char *qol_temp_strdup(const char *cstr) {
        size_t n = strlen(cstr);
        char *result = qol_temp_alloc(n + 1);
        QOL_ASSERT(result != NULL && "Increase QOL_TEMP_CAPACITY");
        memcpy(result, cstr, n);
        result[n] = '\0';
        return result;
    }

    void *qol_temp_alloc(size_t size) {
        if (qol_temp_size + size > QOL_TEMP_CAPACITY) return NULL;
        void *result = &qol_temp[qol_temp_size];
        qol_temp_size += size;
        return result;
    }

    char *qol_temp_sprintf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        int n = vsnprintf(NULL, 0, format, args);
        va_end(args);

        QOL_ASSERT(n >= 0);
        char *result = qol_temp_alloc(n + 1);
        QOL_ASSERT(result != NULL && "Extend the size of the temporary allocator");
        va_start(args, format);
        vsnprintf(result, n + 1, format, args);
        va_end(args);

        return result;
    }

    void qol_temp_reset(void) {
        qol_temp_size = 0;
    }

    size_t qol_temp_save(void) {
        return qol_temp_size;
    }

    void qol_temp_rewind(size_t checkpoint) {
        qol_temp_size = checkpoint;
    }

    //////////////////////////////////////////////////
    /// FILE_OPS /////////////////////////////////////
    //////////////////////////////////////////////////

    bool qol_mkdir_if_not_exists(const char *path) {
        struct stat st;
#if defined(WINDOWS)
        if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) {
            return true;
        }
#else
        if (stat(path, &st) == 0) {
            return true;
        }
#endif
        return qol_mkdir(path);
    }

    bool qol_mkdir(const char *path) {
#ifdef _WIN32
        int result = _mkdir(path);
#else
        int result = mkdir(path, 0755);
#endif
        if (result != 0) {
            qol_log(QOL_LOG_ERROR, "Failed to create directory: %s\n", path);
            return false;
        }
        qol_log(QOL_LOG_DEBUG, "created directory `%s/`\n", path);
        return true;
    }

    bool qol_copy_file(const char *src_path, const char *dst_path) {
        if (!src_path || !dst_path) return false;

        FILE *src = fopen(src_path, "rb");
        if (!src) {
            qol_log(QOL_LOG_ERROR, "Failed to open source file: %s\n", src_path);
            return false;
        }

        FILE *dst = fopen(dst_path, "wb");
        if (!dst) {
            qol_log(QOL_LOG_ERROR, "Failed to open destination file: %s\n", dst_path);
            fclose(src);
            return false;
        }

        char buffer[4096];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            if (fwrite(buffer, 1, bytes_read, dst) != bytes_read) {
                qol_log(QOL_LOG_ERROR, "Failed to write to destination file\n");
                fclose(src);
                fclose(dst);
                return false;
            }
        }

        fclose(src);
        fclose(dst);
        qol_log(QOL_LOG_DEBUG, "Copied %s to %s\n", src_path, dst_path);
        return true;
    }

    bool qol_copy_dir_rec(const char *src_path, const char *dst_path) {
        if (!src_path || !dst_path) return false;

#if defined(MACOS) || defined(LINUX)
        DIR *dir = opendir(src_path);
        if (!dir) {
            qol_log(QOL_LOG_ERROR, "Failed to open source directory: %s\n", src_path);
            return false;
        }

        if (!qol_mkdir_if_not_exists(dst_path)) {
            closedir(dir);
            return false;
        }

        struct dirent *entry;
        char src_file[1024];
        char dst_file[1024];

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(src_file, sizeof(src_file), "%s/%s", src_path, entry->d_name);
            snprintf(dst_file, sizeof(dst_file), "%s/%s", dst_path, entry->d_name);

            struct stat st;
            if (stat(src_file, &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    if (!qol_copy_dir_rec(src_file, dst_file)) {
                        closedir(dir);
                        return false;
                    }
                } else if (S_ISREG(st.st_mode)) {
                    if (!qol_copy_file(src_file, dst_file)) {
                        closedir(dir);
                        return false;
                    }
                }
            }
        }

        closedir(dir);
        return true;
#elif defined(WINDOWS)
        WIN32_FIND_DATA find_data;
        char search_path[1024];
        snprintf(search_path, sizeof(search_path), "%s\\*", src_path);

        HANDLE handle = FindFirstFile(search_path, &find_data);
        if (handle == INVALID_HANDLE_VALUE) {
            qol_log(QOL_LOG_ERROR, "Failed to open source directory: %s\n", src_path);
            return false;
        }

        if (!qol_mkdir_if_not_exists(dst_path)) {
            FindClose(handle);
            return false;
        }

        do {
            if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
                continue;

            char src_file[1024];
            char dst_file[1024];
            snprintf(src_file, sizeof(src_file), "%s\\%s", src_path, find_data.cFileName);
            snprintf(dst_file, sizeof(dst_file), "%s\\%s", dst_path, find_data.cFileName);

            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!qol_copy_dir_rec(src_file, dst_file)) {
                    FindClose(handle);
                    return false;
                }
            } else {
                if (!qol_copy_file(src_file, dst_file)) {
                    FindClose(handle);
                    return false;
                }
            }
        } while (FindNextFile(handle, &find_data));

        FindClose(handle);
        return true;
#else
        #error Unsupported platform
#endif
    }

    bool qol_read_file(const char *path, QOL_String *content) {
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

            qol_push(content, copy);
        }

        free(line);
        fclose(fp);
        return true;
    }

    bool qol_read_dir(const char *parent, const char *children) {
        if (!parent || !children) return false;
        QOL_UNUSED(children); // Reserved for future filtering

#if defined(MACOS) || defined(LINUX)
        DIR *dir = opendir(parent);
        if (!dir) {
            qol_log(QOL_LOG_ERROR, "Failed to open directory: %s\n", parent);
            return false;
        }

        struct dirent *entry;
        qol_log(QOL_LOG_INFO, "Contents of %s:\n", parent);
        while ((entry = readdir(dir)) != NULL) {
            struct stat st;
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", parent, entry->d_name);

            if (stat(full_path, &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    qol_log(QOL_LOG_INFO, "  [DIR]  %s\n", entry->d_name);
                } else if (S_ISREG(st.st_mode)) {
                    qol_log(QOL_LOG_INFO, "  [FILE] %s (%zu bytes)\n", entry->d_name, (size_t)st.st_size);
                } else {
                    qol_log(QOL_LOG_INFO, "  [????] %s\n", entry->d_name);
                }
            }
        }

        closedir(dir);
        return true;
#elif defined(WINDOWS)
        WIN32_FIND_DATA find_data;
        char search_path[1024];
        snprintf(search_path, sizeof(search_path), "%s\\*", parent);

        HANDLE handle = FindFirstFile(search_path, &find_data);
        if (handle == INVALID_HANDLE_VALUE) {
            qol_log(QOL_LOG_ERROR, "Failed to open directory: %s\n", parent);
            return false;
        }

        qol_log(QOL_LOG_INFO, "Contents of %s:\n", parent);
        do {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                qol_log(QOL_LOG_INFO, "  [DIR]  %s\n", find_data.cFileName);
            } else {
                qol_log(QOL_LOG_INFO, "  [FILE] %s (%lu bytes)\n",
                        find_data.cFileName, find_data.nFileSizeLow);
            }
        } while (FindNextFile(handle, &find_data));

        FindClose(handle);
        return true;
#else
        #error Unsupported platform
#endif
    }

    bool qol_write_file(const char *path, const void *data, size_t size) {
        if (!path || !data) return false;

        FILE *fp = fopen(path, "wb");
        if (!fp) {
            qol_log(QOL_LOG_ERROR, "Failed to open file for writing: %s\n", path);
            return false;
        }

        size_t written = fwrite(data, 1, size, fp);
        fclose(fp);

        if (written != size) {
            qol_log(QOL_LOG_ERROR, "Failed to write all data to file: %s\n", path);
            return false;
        }

        qol_log(QOL_LOG_DEBUG, "Wrote %zu bytes to %s\n", written, path);
        return true;
    }

    const char *qol_get_file_type(const char *path) {
        if (!path) return "unknown";

        const char *dot = strrchr(path, '.');
        if (!dot || dot == path) return "no_ext";

        return dot + 1; // Returns extension without the dot
    }

    bool qol_delete_file(const char *path) {
        if (!path) return false;

#if defined(MACOS) || defined(LINUX)
        if (unlink(path) != 0) {
            qol_log(QOL_LOG_ERROR, "Failed to delete file: %s\n", path);
            return false;
        }

        qol_log(QOL_LOG_DEBUG, "Deleted file: %s\n", path);
        return true;
#elif defined(WINDOWS)
        if (DeleteFile(path) == 0) {
            qol_log(QOL_LOG_ERROR, "Failed to delete file: %s\n", path);
            return false;
        }

        qol_log(QOL_LOG_DEBUG, "Deleted file: %s\n", path);
        return true;
#else
        #error Unsupported platform
#endif
    }

    bool qol_delete_dir(const char *path) {
        if (!path) return false;

#if defined(MACOS) || defined(LINUX)
        DIR *dir = opendir(path);
        if (!dir) {
            qol_log(QOL_LOG_ERROR, "Failed to open directory for deletion: %s\n", path);
            return false;
        }

        struct dirent *entry;
        char full_path[1024];

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

            struct stat st;
            if (stat(full_path, &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    qol_delete_dir(full_path);
                } else if (S_ISREG(st.st_mode)) {
                    qol_delete_file(full_path);
                }
            }
        }

        closedir(dir);
        if (rmdir(path) != 0) {
            qol_log(QOL_LOG_ERROR, "Failed to remove directory: %s\n", path);
        } else {
            qol_log(QOL_LOG_DEBUG, "Removed directory: %s\n", path);
        }
        return true;
#elif defined(WINDOWS)
        WIN32_FIND_DATA find_data;
        char search_path[1024];
        snprintf(search_path, sizeof(search_path), "%s\\*", path);

        HANDLE handle = FindFirstFile(search_path, &find_data);
        if (handle == INVALID_HANDLE_VALUE) {
            qol_log(QOL_LOG_ERROR, "Failed to open directory for deletion: %s\n", path);
            return false;
        }

        do {
            if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
                continue;

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s\\%s", path, find_data.cFileName);

            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                qol_delete_dir(full_path);
            } else {
                qol_delete_file(full_path);
            }
        } while (FindNextFile(handle, &find_data));

        FindClose(handle);
        if (RemoveDirectory(path) == 0) {
            qol_log(QOL_LOG_ERROR, "Failed to remove directory: %s\n", path);
        } else {
            qol_log(QOL_LOG_DEBUG, "Removed directory: %s\n", path);
        }
        return true;
#else
        #error Unsupported platform
#endif

    }

    void qol_release_string(QOL_String* content) {
        if (!content || !content->data) return;

        for (size_t i = 0; i < content->len; i++) {
            free(content->data[i]);
        }
        free(content->data);
        content->data = NULL;
        content->len = content->cap = 0;
    }

    // Path utilities
    const char *qol_path_name(const char *path) {
#ifdef WINDOWS
        const char *p1 = strrchr(path, '/');
        const char *p2 = strrchr(path, '\\');
        const char *p = (p1 > p2)? p1 : p2;
        return p ? p + 1 : path;
#else
        const char *p = strrchr(path, '/');
        return p ? p + 1 : path;
#endif
    }

    bool qol_rename(const char *old_path, const char *new_path) {
        qol_log(QOL_LOG_INFO, "renaming %s -> %s\n", old_path, new_path);
#ifdef WINDOWS
        if (!MoveFileEx(old_path, new_path, MOVEFILE_REPLACE_EXISTING)) {
            qol_log(QOL_LOG_ERROR, "could not rename %s to %s: %s\n", old_path, new_path, qol_win32_error_message(GetLastError()));
            return false;
        }
#else
        if (rename(old_path, new_path) < 0) {
            qol_log(QOL_LOG_ERROR, "could not rename %s to %s: %s\n", old_path, new_path, strerror(errno));
            return false;
        }
#endif
        return true;
    }

    const char *qol_get_current_dir_temp(void) {
#ifdef WINDOWS
        DWORD nBufferLength = GetCurrentDirectory(0, NULL);
        if (nBufferLength == 0) {
            qol_log(QOL_LOG_ERROR, "could not get current directory: %s\n", qol_win32_error_message(GetLastError()));
            return NULL;
        }

        char *buffer = (char*) qol_temp_alloc(nBufferLength);
        if (GetCurrentDirectory(nBufferLength, buffer) == 0) {
            qol_log(QOL_LOG_ERROR, "could not get current directory: %s\n", qol_win32_error_message(GetLastError()));
            return NULL;
        }

        return buffer;
#else
        char *buffer = (char*) qol_temp_alloc(PATH_MAX);
        if (getcwd(buffer, PATH_MAX) == NULL) {
            qol_log(QOL_LOG_ERROR, "could not get current directory: %s\n", strerror(errno));
            return NULL;
        }

        return buffer;
#endif
    }

    bool qol_set_current_dir(const char *path) {
#ifdef WINDOWS
        if (!SetCurrentDirectory(path)) {
            qol_log(QOL_LOG_ERROR, "could not set current directory to %s: %s\n", path, qol_win32_error_message(GetLastError()));
            return false;
        }
        return true;
#else
        if (chdir(path) < 0) {
            qol_log(QOL_LOG_ERROR, "could not set current directory to %s: %s\n", path, strerror(errno));
            return false;
        }
        return true;
#endif
    }

    int qol_file_exists(const char *file_path) {
#ifdef WINDOWS
        DWORD dwAttrib = GetFileAttributesA(file_path);
        return dwAttrib != INVALID_FILE_ATTRIBUTES;
#else
        struct stat statbuf;
        if (stat(file_path, &statbuf) < 0) {
            if (errno == ENOENT) return 0;
            qol_log(QOL_LOG_ERROR, "Could not check if file %s exists: %s\n", file_path, strerror(errno));
            return -1;
        }
        return 1;
#endif
    }

    // Rebuild detection
    int qol_needs_rebuild(const char *output_path, const char **input_paths, size_t input_paths_count) {
#ifdef WINDOWS
        BOOL bSuccess;

        HANDLE output_path_fd = CreateFile(output_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
        if (output_path_fd == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) return 1;
            qol_log(QOL_LOG_ERROR, "Could not open file %s: %s\n", output_path, qol_win32_error_message(GetLastError()));
            return -1;
        }
        FILETIME output_path_time;
        bSuccess = GetFileTime(output_path_fd, NULL, NULL, &output_path_time);
        CloseHandle(output_path_fd);
        if (!bSuccess) {
            qol_log(QOL_LOG_ERROR, "Could not get time of %s: %s\n", output_path, qol_win32_error_message(GetLastError()));
            return -1;
        }

        for (size_t i = 0; i < input_paths_count; ++i) {
            const char *input_path = input_paths[i];
            HANDLE input_path_fd = CreateFile(input_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
            if (input_path_fd == INVALID_HANDLE_VALUE) {
                qol_log(QOL_LOG_ERROR, "Could not open file %s: %s\n", input_path, qol_win32_error_message(GetLastError()));
                return -1;
            }
            FILETIME input_path_time;
            bSuccess = GetFileTime(input_path_fd, NULL, NULL, &input_path_time);
            CloseHandle(input_path_fd);
            if (!bSuccess) {
                qol_log(QOL_LOG_ERROR, "Could not get time of %s: %s\n", input_path, qol_win32_error_message(GetLastError()));
                return -1;
            }

            if (CompareFileTime(&input_path_time, &output_path_time) == 1) return 1;
        }

        return 0;
#else
        struct stat statbuf = {0};

        if (stat(output_path, &statbuf) < 0) {
            if (errno == ENOENT) return 1;
            qol_log(QOL_LOG_ERROR, "could not stat %s: %s\n", output_path, strerror(errno));
            return -1;
        }
        int output_path_time = statbuf.st_mtime;

        for (size_t i = 0; i < input_paths_count; ++i) {
            const char *input_path = input_paths[i];
            if (stat(input_path, &statbuf) < 0) {
                qol_log(QOL_LOG_ERROR, "could not stat %s: %s\n", input_path, strerror(errno));
                return -1;
            }
            int input_path_time = statbuf.st_mtime;
            if (input_path_time > output_path_time) return 1;
        }

        return 0;
#endif
    }

    int qol_needs_rebuild1(const char *output_path, const char *input_path) {
        return qol_needs_rebuild(output_path, &input_path, 1);
    }

    //////////////////////////////////////////////////
    /// HASHMAP //////////////////////////////////////
    //////////////////////////////////////////////////

    static size_t qol_hm_hash(void* key, size_t key_size, size_t capacity) {
        size_t hash = 5381;
        const unsigned char *p = (const unsigned char *)key;
        for (size_t i = 0; i < key_size; i++) {
            hash = ((hash << 5) + hash) + p[i];
        }
        return hash % capacity;
    }

    static bool qol_hm_keys_equal(void* key1, void* key2) {
        size_t key_size = strlen(key1) + 1;
        return memcmp(key1, key2, key_size) == 0;
    }

    QOL_HashMap* qol_hm_create() {
        QOL_HashMap* hm = (QOL_HashMap*)calloc(1, sizeof(QOL_HashMap));
        if (!hm) return NULL;

        int initial_capacity = 4;
        hm->buckets = (QOL_HashMapEntry*)calloc(initial_capacity, sizeof(QOL_HashMapEntry));
        if (!hm->buckets) {
            free(hm);
            return NULL;
        }

        hm->capacity = initial_capacity;
        hm->size = 0;
        return hm;
    }

    static void qol_hm_resize(QOL_HashMap* hm) {
        size_t old_capacity = hm->capacity;
        QOL_HashMapEntry* old_buckets = hm->buckets;

        hm->capacity = hm->capacity * 2;
        hm->buckets = (QOL_HashMapEntry*)calloc(hm->capacity, sizeof(QOL_HashMapEntry));
        if (!hm->buckets) {
            hm->buckets = old_buckets;
            hm->capacity = old_capacity;
            qol_log(QOL_LOG_ERROR, "Failed to resize hashmap\n");
            return;
        }

        size_t new_size = 0;
        for (size_t i = 0; i < old_capacity; i++) {
            if (old_buckets[i].state == QOL_HM_USED) {
                size_t hash = qol_hm_hash(old_buckets[i].key, old_buckets[i].key_size, hm->capacity);
                size_t index = hash;

                // Linear probing to find empty slot
                while (hm->buckets[index].state == QOL_HM_USED) {
                    index = (index + 1) % hm->capacity;
                    if (index == hash) {
                        qol_log(QOL_LOG_ERROR, "Hashmap table is full during resize\n");
                        break;
                    }
                }

                if (hm->buckets[index].state != QOL_HM_USED) {
                    hm->buckets[index].key = old_buckets[i].key;
                    hm->buckets[index].value = old_buckets[i].value;
                    hm->buckets[index].key_size = old_buckets[i].key_size;
                    hm->buckets[index].value_size = old_buckets[i].value_size;
                    hm->buckets[index].state = QOL_HM_USED;
                    new_size++;
                }
            }
        }

        free(old_buckets);
        hm->size = new_size;
        qol_log(QOL_LOG_DEBUG, "Hashmap resized to %zu buckets\n", hm->capacity);
    }

    void qol_hm_put(QOL_HashMap* hm, void* key, void* value) {
        if (!hm || !key || !value) return;

        // Keys are always strings (null-terminated)
        size_t key_size = strlen(key) + 1;

        // NOTE: Values are stored as pointers only, not copied
        // This assumes caller manages value lifetime
        size_t value_size = sizeof(void*);

        // Resize if load factor > 0.75
        if (hm->size * 4 > hm->capacity * 3) {
            qol_hm_resize(hm);
        }

        size_t hash = qol_hm_hash(key, key_size, hm->capacity);
        size_t index = hash;

        // Linear probing
        while (hm->buckets[index].state != QOL_HM_EMPTY) {
            if (hm->buckets[index].state == QOL_HM_USED && qol_hm_keys_equal(hm->buckets[index].key, key)) {
                qol_log(QOL_LOG_DEBUG, "Updating entry for key: %s\n", (const char*)key);
                // Update existing value - just store the pointer
                free(hm->buckets[index].value);
                hm->buckets[index].value = malloc(value_size);
                if (hm->buckets[index].value) {
                    memcpy(hm->buckets[index].value, &value, value_size);
                    hm->buckets[index].value_size = value_size;
                }
                return;
            }
            index = (index + 1) % hm->capacity;
            if (index == hash) {
                qol_log(QOL_LOG_ERROR, "Hashmap table is full\n");
                return;
            }
        }

        // Found empty or deleted slot
        if (hm->buckets[index].state == QOL_HM_EMPTY || hm->buckets[index].state == QOL_HM_DELETED) {
            qol_log(QOL_LOG_DEBUG, "Inserting new entry for key: %s\n", (const char*)key);

            hm->buckets[index].key = malloc(key_size);
            hm->buckets[index].value = malloc(value_size);

            if (!hm->buckets[index].key || !hm->buckets[index].value) {
                if (hm->buckets[index].key) free(hm->buckets[index].key);
                if (hm->buckets[index].value) free(hm->buckets[index].value);
                qol_log(QOL_LOG_ERROR, "Failed to allocate memory for hashmap entry\n");
                return;
            }

            memcpy(hm->buckets[index].key, key, key_size);
            // Store the pointer itself, not the value it points to
            memcpy(hm->buckets[index].value, &value, value_size);
            hm->buckets[index].key_size = key_size;
            hm->buckets[index].value_size = value_size;
            hm->buckets[index].state = QOL_HM_USED;
            hm->size++;
        }
    }

    void* qol_hm_get(QOL_HashMap* hm, void* key) {
        if (!hm || !key) return NULL;

        size_t key_size = strlen(key) + 1;
        size_t hash = qol_hm_hash(key, key_size, hm->capacity);
        size_t index = hash;

        // Linear probing
        while (hm->buckets[index].state != QOL_HM_EMPTY) {
            if (hm->buckets[index].state == QOL_HM_USED && qol_hm_keys_equal(hm->buckets[index].key, key)) {
                // value is a pointer to the actual value pointer
                void** value_ptr = (void**)hm->buckets[index].value;
                return value_ptr ? *value_ptr : NULL;
            }
            index = (index + 1) % hm->capacity;
            if (index == hash) break; // Searched the entire table
        }

        return NULL;
    }

    bool qol_hm_contains(QOL_HashMap* hm, void* key) {
        return qol_hm_get(hm, key) != NULL ? true : false;
    }

    bool qol_hm_remove(QOL_HashMap* hm, void* key) {
        if (!hm || !key) return false;

        size_t key_size = strlen(key) + 1;
        size_t hash = qol_hm_hash(key, key_size, hm->capacity);
        size_t index = hash;

        // Linear probing
        while (hm->buckets[index].state != QOL_HM_EMPTY) {
            if (hm->buckets[index].state == QOL_HM_USED && qol_hm_keys_equal(hm->buckets[index].key, key)) {
                // Mark as deleted
                free(hm->buckets[index].key);
                free(hm->buckets[index].value);
                hm->buckets[index].key = NULL;
                hm->buckets[index].value = NULL;
                hm->buckets[index].state = QOL_HM_DELETED;
                hm->size--;
                return true;
            }
            index = (index + 1) % hm->capacity;
            if (index == hash) break;
        }

        return false;
    }

    void qol_hm_clear(QOL_HashMap* hm) {
        if (!hm) return;

        for (size_t i = 0; i < hm->capacity; i++) {
            if (hm->buckets[i].state == QOL_HM_USED) {
                free(hm->buckets[i].key);
                free(hm->buckets[i].value);
                hm->buckets[i].key = NULL;
                hm->buckets[i].value = NULL;
                hm->buckets[i].state = QOL_HM_EMPTY;
            }
        }
        hm->size = 0;
    }

    void qol_hm_release(QOL_HashMap* hm) {
        if (!hm) return;
        qol_hm_clear(hm);
        free(hm->buckets);
        free(hm);
    }

    size_t qol_hm_size(QOL_HashMap* hm) {
        return hm ? hm->size : 0;
    }

    bool qol_hm_empty(QOL_HashMap* hm) {
        return !hm || hm->size == 0;
    }

    //////////////////////////////////////////////////
    /// UNITTEST /////////////////////////////////////
    //////////////////////////////////////////////////

    typedef struct {
        qol_test_t tests[1024];
        size_t count;
        size_t passed;
        size_t failed;
    } qol_test_suite_t;

    static qol_test_suite_t qol_test_suite = {0};
    char qol_test_failure_msg[256] = {0};

    void qol_test_register(const char *name, const char *file, int line, void (*test_func)(void)) {
        if (qol_test_suite.count >= QOL_ARRAY_LEN(qol_test_suite.tests)) {
            fprintf(stderr, "Too many tests registered!\n");
            return;
        }
        qol_test_t *test = &qol_test_suite.tests[qol_test_suite.count++];
        test->name = name;
        test->file = file;
        test->line = line;
        test->func = test_func;
    }

    static bool qol_test_current_failed = false;

    void qol_test_fail(void) {
        qol_test_current_failed = true;
    }

    int qol_test_run_all(void) {
        size_t test_count = qol_test_suite.count;
        qol_test_suite.passed = 0;
        qol_test_suite.failed = 0;

        // Find the longest test name for alignment
        size_t max_name_len = 0;
        for (size_t i = 0; i < test_count; i++) {
            size_t len = strlen(qol_test_suite.tests[i].name);
            if (len > max_name_len) max_name_len = len;
        }

        const size_t target_width = 60;
        const size_t prefix_len = 7; // "[TEST] " prefix

        for (size_t i = 0; i < test_count; i++) {
            qol_test_t *test = &qol_test_suite.tests[i];
            qol_test_current_failed = false;
            qol_test_failure_msg[0] = '\0'; // Reset failure message

            // Calculate dots needed to reach alignment point
            size_t name_len = strlen(test->name);
            size_t total_prefix = prefix_len + name_len;
            size_t space_needed = target_width - total_prefix;
            // size_t dots_needed = total_prefix < target_width ? target_width - total_prefix : 10;
            size_t dots_needed = space_needed;

            printf("[TEST] %s ", test->name);

            // Print dots for alignment
            for (size_t j = 0; j < dots_needed; j++) {
                printf(".");
            }

            // Run the test
            test->func();

            // Print result on same line with colors
            if (qol_test_current_failed) {
                printf("\033[31m [FAILED]\033[0m\n"); // Red
                if (qol_test_failure_msg[0] != '\0') {
                    printf("  %s\n", qol_test_failure_msg);
                }
                qol_test_suite.failed++;
            } else {
                printf("\033[32m [OK]\033[0m\n"); // Green
                qol_test_suite.passed++;
            }
        }

        printf("Total: %zu, Passed: %zu, Failed: %zu\n", qol_test_suite.count, qol_test_suite.passed, qol_test_suite.failed);

        return qol_test_suite.failed > 0 ? 1 : 0;
    }

    //////////////////////////////////////////////////
    /// TIMER ////////////////////////////////////////
    //////////////////////////////////////////////////

    void qol_timer_start(QOL_Timer *timer) {
        if (!timer) return;

#if defined(WINDOWS)
        QueryPerformanceFrequency(&timer->frequency);
        QueryPerformanceCounter(&timer->start);
#else
        clock_gettime(CLOCK_MONOTONIC, &timer->start);
#endif
    }

    double qol_timer_elapsed(QOL_Timer *timer) {
        if (!timer) return 0.0;

#if defined(WINDOWS)
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (double)(now.QuadPart - timer->start.QuadPart) / (double)timer->frequency.QuadPart;
#else
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = (double)(now.tv_sec - timer->start.tv_sec) +
                         (double)(now.tv_nsec - timer->start.tv_nsec) / 1e9;
        return elapsed;
#endif
    }

    double qol_timer_elapsed_ms(QOL_Timer *timer) {
        return qol_timer_elapsed(timer) * 1000.0;
    }

    double qol_timer_elapsed_us(QOL_Timer *timer) {
        return qol_timer_elapsed(timer) * 1000000.0;
    }

    uint64_t qol_timer_elapsed_ns(QOL_Timer *timer) {
        if (!timer) return EXIT_SUCCESS;

#if defined(WINDOWS)
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        uint64_t elapsed_ticks = (uint64_t)(now.QuadPart - timer->start.QuadPart);
        // Convert to nanoseconds: (ticks / frequency) * 1e9
        return (uint64_t)((double)elapsed_ticks * 1e9 / (double)timer->frequency.QuadPart);
#else
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        uint64_t elapsed_sec = (uint64_t)(now.tv_sec - timer->start.tv_sec);
        int64_t elapsed_nsec = (int64_t)(now.tv_nsec - timer->start.tv_nsec);
        return elapsed_sec * 1000000000ULL + (uint64_t)elapsed_nsec;
#endif
    }

    void qol_timer_reset(QOL_Timer *timer) {
        if (!timer) return;
        qol_timer_start(timer);
    }

#endif // QOL_IMPLEMENTATION

//////////////////////////////////////////////////
/// QOL_STRIP_PREFIX /////////////////////////////
//////////////////////////////////////////////////

#ifdef QOL_STRIP_PREFIX

    // HELPER
    #define ASSERT                  QOL_ASSERT
    #define UNUSED                  QOL_UNUSED
    #define TODO                    QOL_TODO
    #define UNREACHABLE             QOL_UNREACHABLE
    #define ARRAY_LEN               QOL_ARRAY_LEN
    #define ARRAY_GET               QOL_ARRAY_GET

    // LOGGER
    #define init_logger             qol_init_logger
    #define init_logger_logfile     qol_init_logger_logfile
    #define get_time                qol_get_time
    #define TIME                    QOL_TIME
    #define debug                   qol_debug
    #define info                    qol_info
    #define cmd                     qol_cmd
    #define hint                    qol_hint
    #define warn                    qol_warn
    #define error                   qol_error
    #define critical                qol_critical
    #define LOG_NONE                QOL_LOG_NONE
    #define LOG_DEBUG               QOL_LOG_DEBUG
    #define LOG_INFO                QOL_LOG_INFO
    #define LOG_CMD                 QOL_LOG_CMD
    #define LOG_HINT                QOL_LOG_HINT
    #define LOG_WARN                QOL_LOG_WARN
    #define LOG_ERROR               QOL_LOG_ERROR
    #define LOG_CRITICAL            QOL_LOG_CRITICAL

    // CLI_PARSER
    #define init_argparser          qol_init_argparser
    #define add_argument            qol_add_argument
    #define get_argument            qol_get_argument
    #define shift                   qol_shift
    #define arg_t                   qol_arg_t

    // NO_BUILD
    #define CmdTask                 QOL_CmdTask
    #define Proc                    QOL_Proc
    #define INVALID_PROC            QOL_INVALID_PROC
    #define auto_rebuild            qol_auto_rebuild
    #define auto_rebuild_plus       qol_auto_rebuild_plus
    #define get_filename_no_ext     qol_get_filename_no_ext
    #define default_compiler_flags  qol_default_compiler_flags
    #define default_c_build         qol_default_c_build
    #define run                     qol_run
    #define run_always              qol_run_always
    #define proc_wait               qol_proc_wait
    #define procs_wait              qol_procs_wait
    #define Cmd                     QOL_Cmd
    #define Procs                   QOL_Procs
    #define RunOptions              QOL_RunOptions

    // DYN_ARRAY
    #define grow                    qol_grow
    #define shrink                  qol_shrink
    #define push                    qol_push
    #define drop                    qol_drop
    #define dropn                   qol_dropn
    #define resize                  qol_resize
    #define release                 qol_release
    #define back                    qol_back
    #define swap                    qol_swap
    #define list                    qol_list

    // FILE_OPS
    #define String                  QOL_String
    #define mkdir                   qol_mkdir
    #define mkdir_if_not_exists     qol_mkdir_if_not_exists
    #define copy_file               qol_copy_file
    #define copy_dir_rec            qol_copy_dir_rec
    #define read_dir                qol_read_dir
    #define read_file               qol_read_file
    #define write_file              qol_write_file
    #define get_file_type           qol_get_file_type
    #define delete_file             qol_delete_file
    #define delete_dir              qol_delete_dir
    #define release_string          qol_release_string
    #define path_name               qol_path_name
    #define rename                  qol_rename
    #define get_current_dir_temp    qol_get_current_dir_temp
    #define set_current_dir         qol_set_current_dir
    #define file_exists             qol_file_exists
    #define needs_rebuild           qol_needs_rebuild
    #define needs_rebuild1          qol_needs_rebuild1

    // TEMP_ALLOCATOR
    #define temp_strdup             qol_temp_strdup
    #define temp_alloc              qol_temp_alloc
    #define temp_sprintf            qol_temp_sprintf
    #define temp_reset              qol_temp_reset
    #define temp_save               qol_temp_save
    #define temp_rewind             qol_temp_rewind

    // HASHMAP
    #define HashMap                 QOL_HashMap
    #define HashMapEntry            QOL_HashMapEntry
    #define hm_create               qol_hm_create
    #define hm_put                  qol_hm_put
    #define hm_get                  qol_hm_get
    #define hm_contains             qol_hm_contains
    #define hm_remove               qol_hm_remove
    #define hm_clear                qol_hm_clear
    #define hm_release              qol_hm_release
    #define hm_size                 qol_hm_size
    #define hm_empty                qol_hm_empty

    // UNITTEST
    #define Test                    qol_test_t
    #define test_register           qol_test_register
    #define test_run_all            qol_test_run_all
    #define test_print_summary      qol_test_print_summary
    #define TEST_ASSERT             QOL_TEST_ASSERT
    #define TEST_EQ                 QOL_TEST_EQ
    #define TEST_NEQ                QOL_TEST_NEQ
    #define TEST_STREQ              QOL_TEST_STREQ
    #define TEST_STRNEQ             QOL_TEST_STRNEQ
    #define TEST_TRUTHY             QOL_TEST_TRUTHY
    #define TEST_FALSY              QOL_TEST_FALSY
    #define TEST                    QOL_TEST

    // TIMER
    #define Timer                   QOL_Timer
    #define timer_start             qol_timer_start
    #define timer_elapsed           qol_timer_elapsed
    #define timer_elapsed_ms        qol_timer_elapsed_ms
    #define timer_elapsed_us        qol_timer_elapsed_us
    #define timer_elapsed_ns        qol_timer_elapsed_ns
    #define timer_reset             qol_timer_reset

    // ANSI COLORS
    #define enable_ansi             QOL_enable_ansi

#endif // QOL_STRIP_PREFIX

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QOL_BUILD_H
