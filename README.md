# build.h

A single-header **quality-of-life (QoL) utility library for C**. It provides logging, CLI argument parsing, dynamic arrays, and lightweight build-system helpers in a portable way (Linux, macOS, Windows).

## Features

### Logger

  - Configurable log levels (`DEBUG`, `INFO`, `WARN`, `ERROR`, etc.)
  - Optional colored output
  - Optional timestamps
  - Can integrate with your own logging system or fall back to `printf`.

### CLI Argument Parser

  - Register arguments with long/short forms.
  - Supports default values.
  - Auto-generates a `--help` message.
  - Works cross-platform with `argc, argv`.

### Build Utilities

  - Define build tasks (`SHL_BuildConfig`).
  - Dispatch builds with auto-detected compiler.
  - Optional auto-run of generated executables.
  - Async/multi-threaded builds (planned).
  - Auto-rebuild support (works on Linux/macOS, partial on Windows).

### Dynamic Arrays

  - Lightweight macro-based "vector-like" container.
  - `push`, `drop`, `resize`, `shrink`, `release`, etc.
  - Minimal overhead, C89-compatible.
  - Type-safe via `shl_list(T)` macro.

### Helper Macros

  - `SHL_ASSERT`, `SHL_UNUSED`, `SHL_ARRAY_LEN`, `SHL_TODO`, `SHL_UNREACHABLE`.
  - Strip-prefix mode (`#define SHL_STRIP_PREFIX`) for shorter names like `debug()`, `push()`, etc.

## Getting Started

### Include

```c
#define SHL_IMPLEMENTATION
#include "build.h"
```

### Example: Logger

```c
SHL_LogConfig_t cfg = { SHL_LOG_DEBUG, true, true };
shl_init_logger(cfg);

shl_info("Hello from build.h!");
shl_warn("This is a warning");
```

### Example: CLI Parser

```c
int main(int argc, char *argv[]) {
    shl_add_argument("--name", "world", "Name to greet");
    shl_init_argparser(argc, argv);

    shl_arg_t *arg = shl_get_argument("--name");
    printf("Hello, %s!\n", shl_arg_as_string(arg));
    return 0;
}
```

### Example: Build Config

```c
SHL_BuildConfig cfg = {
    .source = "main.c",
    .output = "app",
    .compiler = "gcc",
    .compiler_flags = "-Wall -O2",
    .autorun = true
};
shl_dispatch_build(&cfg);
```

### Example: Dynamic Array

```c
shl_list(int) nums = {0};
shl_push(&nums, 42);
shl_push(&nums, 100);
printf("Last: %d\n", shl_back(&nums));
shl_release(&nums);
```

##  Roadmap

> Not sure if the single header build.h is a good choice, some user might only want a logger, others only want a cli parser. Might be changed in the future.

- [x] Logger
- [x] No Build
- [x] Dynamic Arrays
- [x] Helper Macros
- [x] CLI Argument Parser
- [ ] Hashmap
- [ ] String
- [ ] Queue and stack macros
- [ ] Ring buffer
- [ ] linked list
- [ ] Timing utilities (high-resolution timers, benchmarks)
- [ ] Memory utilities (arena allocator / bump allocator).
- [ ] Unit-test runner integration
- [ ] Easier Parallelization
- [ ] Better Error Handling

## Installation

Just drop `build.h` into your project.
To define implementation once, add:

```c
#define SHL_IMPLEMENTATION
#include "build.h"
```

All other translation units should just `#include "build.h"` without defining `SHL_IMPLEMENTATION`.

## License

Copyright (c) 2025 **Raphaele Salvatore Licciardo**
All rights reserved.
