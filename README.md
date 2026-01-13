# build.h

**Single-header quality-of-life utilities for C.** Pragmatic. Portable. No nonsense.

A collection of essential utilities that make C development more pleasant. Think of it as a mix between [stb](https://github.com/nothings/stb) and [nob.h](https://github.com/tsoding/nob.h) — everything you need in one header file.

## Features

- **Logger** with levels, colors, and timestamps
- **ANSI color support** with macros for foreground/background colors, text attributes, 256-color, and RGB truecolor
- **CLI arg parser** with simple long/short flags
- **Dynamic array macros** (`grow`, `push`, etc.)
- **HashMap** for string keys to pointer values
- **File operations** (mkdir, copy files/dirs, read/write files, list dirs)
- **High-resolution timers** for precise benchmarking and timing
- **Build helpers**: rebuild self when sources change, run simple builds
- **Unit test harness** with minimal macros
- **Temporary allocator** for short-lived allocations without manual cleanup
- **Auto-free** for automatic memory cleanup using GCC/Clang cleanup attribute
- **Path utilities** for common path manipulations
- **String utilities** for common string operations (trim, split, join, replace, etc.)
- **Cross-platform command execution** using fork/exec (POSIX) or CreateProcess (Windows)
- **Thread-safe** implementation throughout with mutexes

**Supported platforms:** Linux, macOS, Windows

## Installation

Drop `build.h` into your project:

```bash
wget https://raw.githubusercontent.com/RaphaeleL/build.h/refs/heads/main/build.h
```

In exactly **one** `.c` file, before including `build.h`, define `QOL_IMPLEMENTATION`. Optionally define `QOL_STRIP_PREFIX` to remove the `qol_` prefix from public names for cleaner code:

```c
#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX  // Optional: use short names like `info` instead of `qol_info`
#include "./build.h"
```

## Quick Start

### Tiny Build Script

This `build.c` recompiles itself when it changes and builds `main.c` to `./main`:

```c
#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "./build.h"

int main(void) {
    auto_rebuild(__FILE__);

    Cmd b = default_c_build("main.c", "main");
    push(&b, "-Wall", "-Wextra");  // add compiler flags using variadic push
    if (!run(&b)) {  // auto-releases on success or failure
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

Compile and run:

```bash
cc -o build build.c && ./build
```

## Build Helpers

The build helpers provide a simple way to compile C programs without a traditional build system. They automatically check if source files are newer than outputs and only rebuild when necessary.

**Core functions:**

- **`default_c_build(source, output)`** — Returns a `QOL_Cmd` (dynamic array) with platform defaults: `[compiler, flags, source, "-o", output]`
- **`run(&cmd)`** or **`run(&cmd, .procs=&procs)`** — Builds only if `source` is newer than `output` (extracts source/output from command array). Supports both sync and async execution
- **`run_always(&cmd)`** or **`run_always(&cmd, .procs=&procs)`** — Always builds (no timestamp check). Supports both sync and async execution
- **`auto_rebuild(src)`** — If `src` changed, rebuilds current binary, then re-executes it
- **`auto_rebuild_plus(src, ...)`** — Like above but also checks additional dependency paths (variadic, terminated with `NULL`; macro appends the terminator for you)
- **`needs_rebuild(output_path, input_paths, count)`** — Checks if rebuild is needed by comparing timestamps. Returns `1` if rebuild needed, `0` if up-to-date, `-1` on error. Handles multiple input files
- **`needs_rebuild1(output_path, input_path)`** — Convenience wrapper for single input file

**Async execution helpers:**

- **`proc_wait(proc)`** — Wait for an async process to complete. Returns `true` on success, `false` on failure
- **`procs_wait(&procs)`** — Wait for all processes in a `Procs` array to complete. Returns `true` if all succeed, `false` otherwise

### Async Execution

Both `run()` and `run_always()` support asynchronous execution. By default, they run synchronously (wait for completion), maintaining backward compatibility. To enable async mode, set the `async` field on the command and pass a `Procs` array using designated initializer syntax:

```c
// Sync mode (default, backward compatible)
Cmd cmd = default_c_build("main.c", "main");
if (!run(&cmd)) {  // waits for completion
    return EXIT_FAILURE;
}

// Async mode - run multiple builds in parallel
Procs procs = {0};

Cmd build1 = default_c_build("file1.c", "file1");
build1.async = true;
run_always(&build1, .procs=&procs);  // process handle automatically added to procs

Cmd build2 = default_c_build("file2.c", "file2");
build2.async = true;
run_always(&build2, .procs=&procs);  // process handle automatically added to procs

// Do other work while builds run...

// Wait for all builds to complete
if (!procs_wait(&procs)) {
    return EXIT_FAILURE;
}
```

**Notes:**
- Use designated initializer syntax: `run(&cmd, .procs=&procs)` to track async processes
- The `procs` parameter is optional — omit it for sync mode or when you don't need to track processes
- When `async=true` and `procs` is provided, process handles are automatically added to the `procs` array
- Use `procs_wait(&procs)` to wait for all tracked processes to complete
- Cross-platform compatible: uses `CreateProcess`/`WaitForSingleObject` on Windows, `fork`/`execvp`/`waitpid` on Unix

`QOL_Cmd` is a dynamic array structure (`data`, `len`, `cap`) — use the dynamic array macros (`push`, `release`, etc.) to build commands:

```c
Cmd cfg = default_c_build("main.c", "main");
push(&cfg, "-Wall", "-Wextra", "-Iinclude");  // variadic push: add multiple flags at once
if (!run(&cfg)) {  // auto-releases on success or failure
    return EXIT_FAILURE;
}
// Note: use run_always(&cfg) if you need to keep the command after building
```

Or build from scratch:

```c
Cmd cmd = {0};
push(&cmd, "cc", "-Wall", "-Wextra");  // variadic push for compiler and flags
push(&cmd, "main.c", "-o", "main");
run_always(&cmd);  // or run_always(&cmd, .procs=&procs) for async
release(&cmd);
```

## Logger

Simple, colorful logging with levels and timestamps:

```c
init_logger(.level=LOG_INFO, .time=true, .color=true, .time_color=true);
debug("debug: %d\n", 1);
info("info\n");
warn("warning\n");
error("fatal error message\n");  // exits
```

**Log levels:** `LOG_DIAG`, `LOG_INFO`, `LOG_EXEC`, `LOG_HINT`, `LOG_WARN`, `LOG_ERRO` (exits), `LOG_DEAD` (aborts)

## ANSI Colors

Comprehensive ANSI color support for terminal output. All color codes are available as macros that you can use directly in your code.

**Basic usage:**

```c
printf("%sRed text%s\n", QOL_FG_RED, QOL_RESET);
printf("%sGreen background%s\n", QOL_BG_GREEN, QOL_RESET);
```

**Available macros:**

- Reset codes
- Text attributes (bold, dim, italic, underline, etc.)
- Foreground colors (standard and bright)
- Background colors (standard and bright)
- 256-color support
- Truecolor/RGB support

**Windows support:**

On Windows, call `QOL_enable_ansi()` once at program startup to enable ANSI color support in the console:

```c
#define QOL_IMPLEMENTATION
#include "./build.h"

int main(void) {
    QOL_enable_ansi();  // Enable ANSI colors on Windows
    printf("%sHello, colored world!%s\n", QOL_FG_GREEN, QOL_RESET);
    return 0;
}
```

On Linux and macOS, ANSI colors work automatically in terminals that support them.

## CLI Argument Parser

Simple argument parsing with long and short flags:

```c
init_argparser(argc, argv);
add_argument("--threads", "4", "worker threads");
add_argument("--help", NULL, "show help");
arg_t *thr = get_argument("--threads");
info("threads = %s\n", thr ? thr->value : "");
```

**Features:**
- Long flags: `--flag [value]`
- Short flags: `-f [value]` (auto-mapped from the first letter after `--`)
- `--help` automatically prints usage and exits

## Dynamic Arrays

Type-safe dynamic arrays with variadic push support:

```c
list(int) a = {0};
push(&a, 10);              // single value
push(&a, 20, 30);          // multiple values (variadic)
push(&a, 40, 50, 60, 70);  // any number of values
info("len=%zu cap=%zu back=%d\n", a.len, a.cap, back(&a));
drop(&a);
release(&a);
```

**Available macros:** `grow`, `shrink`, `push` (variadic), `drop`, `dropn`, `resize`, `release`, `back`, `swap`, `list(T)`

**Note:** `QOL_Cmd` (used by build helpers) is a dynamic array of `const char*` — use these same macros to build commands dynamically.

## HashMap

String-keyed hash map for pointer values:

```c
HashMap *hm = hm_create();
hm_put(hm, (void*)"name", (void*)"Ada");
void *v = hm_get(hm, (void*)"name");
info("name=%s\n", (char*)v);
hm_remove(hm, (void*)"name");
hm_release(hm);
```

**Notes:**
- Keys are treated as C strings and are copied into the map
- Values are stored as the pointer you pass (the map allocates storage for the pointer, not the pointee). Manage pointee lifetime yourself

## File Operations

Cross-platform file and directory operations:

```c
mkdir_if_not_exists("out");
copy_file("a.txt", "out/a.txt");
copy_dir_rec("assets", "out/assets");
String lines = {0};
read_file("Makefile", &lines);
for (size_t i = 0; i < lines.len; i++) info("%s\n", lines.data[i]);
release_string(&lines);
```

**Available functions:** `read_dir(path, filter)`, `write_file(path, data, size)`, `get_file_type(path)`, `delete_file(path)`

## Path Utilities

Common path manipulation functions:

```c
const char *name = path_name("/path/to/file.txt");  // returns "file.txt"
const char *cwd = get_current_dir_temp();           // get current directory (uses temp allocator)
set_current_dir("subdir");                          // change directory
rename("old.txt", "new.txt");                       // rename file/directory
int exists = file_exists("file.txt");               // returns 1 if exists, 0 if not, -1 on error
```

**Why `get_current_dir_temp()`?** It uses the temporary allocator (see below), so you don't need to free the result. Perfect for short-lived path operations.

## String Utilities

Common string operations for everyday C programming:

```c
// Check prefix/suffix
bool starts = str_starts_with("Hello, World!", "Hello");  // true
bool ends = str_ends_with("Hello, World!", "World!");      // true
bool contains = str_contains("Hello, World!", "World");    // true

// Case-insensitive comparison
int cmp = str_icmp("Hello", "HELLO");  // 0 (equal)

// Trim whitespace (in-place)
char str[] = "   hello   ";
str_trim(str);      // "hello" (both sides)
str_ltrim(str);     // "hello   " (left only)
str_rtrim(str);     // "   hello" (right only)

// Replace substring (returns new string, caller must free)
char *replaced = str_replace("Hello, World!", "World", "Universe");
// "Hello, Universe!"
free(replaced);

// Split string by delimiter
String parts = {0};
str_split("apple,banana,cherry", ',', &parts);
// parts.data[0] = "apple", parts.data[1] = "banana", etc.
release_string(&parts);

// Join strings with separator
String fruits = {0};
push(&fruits, "apple");
push(&fruits, "banana");
char *joined = str_join(&fruits, ", ");  // "apple, banana"
free(joined);
release_string(&fruits);
```

**Available functions:**
- `str_starts_with(str, prefix)` — Check if string starts with prefix
- `str_ends_with(str, suffix)` — Check if string ends with suffix
- `str_trim(str)` — Trim whitespace from both ends (in-place)
- `str_ltrim(str)` — Trim whitespace from left (in-place)
- `str_rtrim(str)` — Trim whitespace from right (in-place)
- `str_replace(str, old_sub, new_sub)` — Replace all occurrences (returns new string)
- `str_split(str, delimiter, result)` — Split string into `QOL_String` array
- `str_join(strings, separator)` — Join `QOL_String` array with separator (returns new string)
- `str_contains(str, substring)` — Check if string contains substring
- `str_icmp(str1, str2)` — Case-insensitive string comparison

## Temporary Allocator

A simple arena-style allocator for short-lived allocations. **Why use it?** No manual `free()` calls needed — perfect for temporary strings, formatted output, and path manipulations that only live for a function call or two.

```c
temp_reset(); // start fresh
char *str1 = temp_strdup("hello");
char *str2 = temp_sprintf("value: %d", 42);
info("%s %s\n", str1, str2); // use them
// No cleanup needed - temp_reset() frees everything

// Checkpoint system for nested scopes
size_t checkpoint = temp_save();
char *temp = temp_strdup("will be freed");
temp_rewind(checkpoint); // frees everything after checkpoint
// 'temp' is now invalid, but earlier allocations remain
```

**Available functions:**
- `temp_strdup(cstr)` — Duplicate a string
- `temp_alloc(size)` — Allocate raw memory
- `temp_sprintf(format, ...)` — Formatted string allocation
- `temp_reset()` — Free all temp memory
- `temp_save()` — Save checkpoint
- `temp_rewind(checkpoint)` — Free memory back to checkpoint

**Use cases:**
- Building temporary file paths: `temp_sprintf("%s/%s", dir, filename)`
- Formatting error messages without worrying about cleanup
- Path manipulations in functions that don't need to return allocated strings
- Any short-lived string operations where manual memory management is annoying

**Important:** The temporary allocator is an *arena allocator* — it doesn't actually "free" memory in the traditional sense. When you call `temp_reset()` or `temp_rewind()`, the memory is marked as reusable, but the data isn't erased. **Pointers become invalid after reset/rewind** — don't use them! The data might still appear to be there until overwritten, but accessing it is undefined behavior.

The allocator uses a fixed-size buffer (8MB by default, configurable via `QOL_TEMP_CAPACITY`). If you need more, increase the capacity or use regular `malloc()`/`free()`.

## Auto-Free

Automatic memory cleanup using GCC/Clang's `__attribute__((cleanup))` extension. Provides RAII-like behavior in C — memory is automatically freed when variables go out of scope, even on early returns or exceptions.

```c
{
    QOL_AUTO_FREE void *ptr = malloc(100);
    QOL_AUTO_FREE int *data = malloc(sizeof(int));
    
    *data = 42;
    // ... use ptr and data ...
    
    // Memory is automatically freed when leaving this scope
    // No need to call free() manually!
}
```

**Usage:**
- Place `QOL_AUTO_FREE` before any pointer variable declaration
- Works with any pointer type (`void*`, `int*`, `char*`, etc.)
- Memory is automatically freed when the variable goes out of scope
- Safe to use with uninitialized pointers (checks for NULL before freeing)

**Example:**

```c
QOL_AUTO_FREE char *buffer = malloc(1024);
strcpy(buffer, "Hello, World!");
info("%s\n", buffer);
// buffer is automatically freed when function returns
```

**Compiler support:**
- **GCC/Clang:** Full support with automatic cleanup
- **Other compilers:** Macro expands to empty (no-op), code still compiles but no auto-free occurs

**Notes:**
- Only works with GCC and Clang compilers that support the `cleanup` attribute
- The cleanup function sets the pointer to NULL after freeing to prevent double-free
- Perfect for reducing memory leaks, especially in error paths where manual cleanup might be forgotten
- Can be combined with regular `malloc()`/`free()` — use auto-free for variables that should be cleaned up on scope exit

## High-Resolution Timers

Precise timing for benchmarking and performance measurement:

```c
Timer t = {0};
timer_start(&t);

// ... do work ...

double elapsed_sec = timer_elapsed(&t);
double elapsed_ms = timer_elapsed_ms(&t);
double elapsed_us = timer_elapsed_us(&t);
uint64_t elapsed_ns = timer_elapsed_ns(&t);

timer_reset(&t);  // restart from now
```

**Available functions:** `timer_start`, `timer_elapsed` (seconds), `timer_elapsed_ms`, `timer_elapsed_us`, `timer_elapsed_ns`, `timer_reset`

**Platform support:**
- Windows: `QueryPerformanceCounter` / `QueryPerformanceFrequency`
- Linux/macOS: `clock_gettime(CLOCK_MONOTONIC)`

## Unit Testing

Minimal test harness with simple macros:

```c
TEST(sample) {
    TEST_EQ(2+2, 4, "math");
}

int main(void) {
    return test_run_all();
}
```

**Available macros:** `TEST`, `TEST_ASSERT`, `TEST_EQ`, `TEST_NEQ`, `TEST_STREQ`, `TEST_STRNEQ`, `TEST_TRUTHY`, `TEST_FALSY`

## Thread Safety

The library is fully thread-safe and can be used safely from multiple threads simultaneously. All global state is protected with cross-platform mutexes that are automatically initialized on first use — no manual setup required.

**Implementation details:**

- **Cross-platform mutexes:** Uses `pthread_mutex_t` on Unix-like systems and `CRITICAL_SECTION` on Windows
- **Per-subsystem mutexes:** Each major subsystem has its own mutex to minimize contention:
  - Logger (`qol_logger_mutex`) — protects logger configuration and file output
  - Temporary allocator (`qol_temp_alloc_mutex`) — protects the arena allocator state
  - Argument parser (`qol_argparser_mutex`) — protects registered arguments and parsing state
  - Test framework (`qol_test_mutex`) — protects test registration and execution
  - Windows error handling (`qol_win32_err_mutex`) — protects Windows error message buffers
- **Thread-local storage:** Time/date buffers (`qol_get_time()`, `qol_get_date()`, `qol_get_datetime()`) and test failure state use thread-local storage to eliminate contention entirely
- **Automatic initialization:** Mutexes are initialized automatically on first use via `qol_init_mutexes()`, which uses atomic operations to ensure thread-safe initialization

**Usage:**

No special setup is required — just use the library from multiple threads:

```c
// Thread 1
qol_log(QOL_LOG_INFO, "Message from thread 1\n");

// Thread 2 (simultaneously)
qol_log(QOL_LOG_INFO, "Message from thread 2\n");

// Both are safe and will be properly serialized
```

All functions that access global state automatically acquire the appropriate mutex, ensuring thread-safe operation without any changes to your code.

## Prefix Stripping

Define `QOL_STRIP_PREFIX` to use short names (e.g., `info` instead of `qol_info`, `Cmd` instead of `QOL_Cmd`). See the bottom of `build.h` for the full mapping.

## Platform Support

**Linux/macOS:** Uses `pthread`, `dirent`, `fork`/`execvp`/`waitpid`, `stat`, `unlink`, `clock_gettime` where needed.

**Windows:** Uses WinAPI (`CreateProcess`, `WaitForSingleObject`, `GetExitCodeProcess`, `FindFirstFile`, `_mkdir`, `DeleteFile`, `QueryPerformanceCounter`).

**Async execution** is fully cross-platform: Windows uses process handles (`HANDLE`), Unix uses process IDs (`pid_t`).

## FAQ

**Q: Why a single header?**  
A: Easier drop-in, no build system glue. Just include and go.

**Q: Can I use this only for logging/arrays/etc.?**  
A: Yes. `auto_rebuild` and build helpers are optional. Use only what you need.

**Q: Is it thread-safe?**  
A: The logger has process-global settings; other parts are not thread-safe. Use appropriate synchronization if needed.

## Roadmap

> Check out the `changelog/` directory for the version history.

**Completed:**
- Logger, Build helpers, Dynamic arrays, CLI parser, File operations, HashMap, Unit test runner, High-res timers, Temporary allocator, Path utilities, String utilities, Cross-platform command execution, Windows error handling, Thread safety, Auto-free

**Planned:**
- Queue/stack macros, ring buffer, linked list, easier parallel builds, better Windows support

## License

MIT. See `LICENSE` file for details.
