# build.h

A single-header **quality-of-life (QoL) utility library for C**. It provides logging, CLI argument parsing, dynamic arrays, and lightweight build-system helpers in a portable way (Linux, macOS, Windows).

## Getting Started

Checkout the `examples\` Directory with many different example usages. Just drop `build.h` into your project. There are several define options, with `#define SHL_IMPLEMENTATION` you enable the default implementation in the `build.h` file. The `#define SHL_STRIP_PREFIX` is removing every `shl_` prefix. And last, the `build.h` is internally using its own logger, if you don't want that, and rather use the plain `printf()` just enable / disable `#define SHL_USE_LOGGER`. If you now include `#include "./build.h"` you should be ready to start!

```c
// File: build.c

// Enable the Implementation in build.h
#define SHL_IMPLEMENTATION

// Strip all shl_ Prefixes
#define SHL_STRIP_PREFIX

// Use the intern Logger
#define SHL_USE_LOGGER

#include "./build.h"

int main() {
    // Auto. detect changes on this file, and auto rebuild during the execution of it
    auto_rebuild();

    // Lets create a Build Config, src/main.c should be compiled to out/main the
    // out/ folder is auto. created. with autorun with are auto. run the executable
    // when we are running this script.
    // thereby we are able to auto detect changes on build.c, compile main.c and run
    // the executable, all with one command ./build
    BuildConfig build = (BuildConfig){
      .source   = "src/main.c",
      .output   = "out/main",
      .autorun  = true
    };

    // lets start the process, oh and it might fail.
    if (!dispatch_build(&build)) return 1;

    return 0;
}
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

## License

Copyright (c) 2025 **Raphaele Salvatore Licciardo**
All rights reserved.
