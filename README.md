## build.h

Single-header utilities for C. Pragmatic. Portable. No nonsense.

- **Logger** with levels, colors, timestamps.
- **CLI arg parser** with simple long/short flags.
- **Dynamic array macros** (`grow`, `push`, etc.).
- **HashMap** for string keys to pointer values.
- **File ops** (mkdir, copy files/dirs, read/write files, list dirs).
- **High-resolution timers** for precise benchmarking and timing.
- **No-build helpers**: rebuild self when sources change, run simple builds.
- **Unit test harness** with minimal macros.

Supported: Linux, macOS, Windows.

### Install

Drop `build.h` into your project.

```bash
wget https://raw.githubusercontent.com/RaphaeleL/build.h/refs/heads/main/build.h
```

In exactly one `.c` file, before including `build.h`, define `SHL_IMPLEMENTATION`. Optionally define `SHL_STRIP_PREFIX` to remove the `shl_` prefix from public names.

```c
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "./build.h"
```

### Quick start: tiny build script

This `build.c` recompiles itself when it changes and builds `main.c` to `./main`.

```c
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "./build.h"

int main(void) {
    auto_rebuild("build.c");

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

### No-build helpers (what actually runs)

- **`default_c_build(source, output)`**: returns a `SHL_Cmd` (dynamic array) with platform defaults: `[compiler, flags, source, "-o", output]`.
- **`run(&cmd)`**: builds only if `source` is newer than `output` (extracts source/output from command array).
- **`run_always(&cmd)`**: always build (no timestamp check).
- **`auto_rebuild(src)`**: if `src` changed, rebuild current binary, then re-exec it.
- **`auto_rebuild_plus(src, ...)`**: like above but also checks additional dependency paths (variadic, terminated with `NULL`; macro appends the terminator for you).

`SHL_Cmd` is a dynamic array structure (`data`, `len`, `cap`) - use the dynamic array macros (`push`, `release`, etc.) to build commands:

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
run_always(&cmd);
release(&cmd);
```

### Logger

```c
init_logger(LOG_DEBUG, /*color*/true, /*time*/true);
debug("debug: %d\n", 1);
info("info\n");
warn("warning\n");
error("fatal error message\n");  // exits
```

Levels: `LOG_DEBUG`, `LOG_INFO`, `LOG_CMD`, `LOG_HINT`, `LOG_WARN`, `LOG_ERROR` (exit), `LOG_CRITICAL` (abort).

### CLI argument parser

```c
init_argparser(argc, argv);
add_argument("--threads", "4", "worker threads");
add_argument("--help", NULL, "show help");
arg_t *thr = get_argument("--threads");
info("threads = %s\n", thr ? thr->value : "");
```

- Long flags: `--flag [value]`
- Short flags: `-f [value]` (auto-mapped from the first letter after `--`)
- `--help` prints usage and exits

### Dynamic array macros

```c
list(int) a = {0};
push(&a, 10);              // single value
push(&a, 20, 30);          // multiple values (variadic)
push(&a, 40, 50, 60, 70);  // any number of values
info("len=%zu cap=%zu back=%d\n", a.len, a.cap, back(&a));
drop(&a);
release(&a);
```

Provided: `grow`, `shrink`, `push` (variadic), `drop`, `dropn`, `resize`, `release`, `back`, `swap`, `list(T)`.

**Note**: `SHL_Cmd` (used by build helpers) is a dynamic array of `const char*` - use these same macros to build commands dynamically.

### HashMap (string keys → pointer values)

```c
HashMap *hm = hm_create();
hm_put(hm, (void*)"name", (void*)"Ada");
void *v = hm_get(hm, (void*)"name");
info("name=%s\n", (char*)v);
hm_remove(hm, (void*)"name");
hm_release(hm);
```

Notes:
- Keys are treated as C strings; they are copied into the map.
- Values are stored as the pointer you pass (the map allocates storage for the pointer, not the pointee). Manage pointee lifetime yourself.

### File operations

```c
mkdir_if_not_exists("out");
copy_file("a.txt", "out/a.txt");
copy_dir_rec("assets", "out/assets");
String lines = {0};
read_file("Makefile", &lines);
for (size_t i = 0; i < lines.len; i++) info("%s\n", lines.data[i]);
release_string(&lines);
```

Also: `read_dir(path, filter)`, `write_file(path, data, size)`, `get_file_type(path)`, `delete_file(path)`.

### High-resolution timers

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

Functions: `timer_start`, `timer_elapsed` (seconds), `timer_elapsed_ms`, `timer_elapsed_us`, `timer_elapsed_ns`, `timer_reset`.

Uses platform-specific high-resolution clocks:
- Windows: `QueryPerformanceCounter` / `QueryPerformanceFrequency`
- Linux/macOS: `clock_gettime(CLOCK_MONOTONIC)`

### Unit testing

```c
TEST(sample) {
    TEST_EQ(2+2, 4, "math");
}

int main(void) {
    return test_run_all();
}
```

Macros: `TEST`, `TEST_ASSERT`, `TEST_EQ`, `TEST_NEQ`, `TEST_STREQ`, `TEST_STRNEQ`, `TEST_TRUTHY`, `TEST_FALSY`.

### Prefix stripping

Define `SHL_STRIP_PREFIX` to use short names (e.g., `info` instead of `shl_info`, `Cmd` instead of `SHL_Cmd`). See the bottom of `build.h` for the full mapping.

### Platform notes

- Linux/macOS: uses `pthread`, `dirent`, `execv`, `stat`, `unlink`, `clock_gettime` where needed.
- Windows: uses WinAPI (`CreateProcess`, `FindFirstFile`, `_mkdir`, `DeleteFile`, `QueryPerformanceCounter`).

### FAQ

- Why a single header? Easier drop-in, no build system glue.
- Can I use this only for logging/arrays/etc.? Yes. `auto_rebuild` and friends are optional.
- Thread safety? The logger has process-global settings; other parts are not thread-safe.

### Roadmap

- Finished: Logger, No-build, Dynamic arrays, Helpers, CLI parser, File ops, HashMap, Unit test runner, High-res timers.
- Planned:  strings, queue/stack macros, ring buffer, linked list, arena/bump allocators, better error handling, easier parallel builds.

### License

MIT. See `LICENSE`.
