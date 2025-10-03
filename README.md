# build.h

A single-header **quality-of-life (QoL) utility library for C**. It provides logging, CLI argument parsing, dynamic arrays, and lightweight build-system helpers in a portable way (Linux, macOS, Windows).

## Getting Started

Checkout the `examples\` Directory with many different example usages. Just drop `build.h` into your project. There are several `define` options, those control the behaviour of `build.h`

- `SHL_USE_SHL_IMPLEMENTATION` - enable the default implementation
- `SHL_USE_LOGGER` - enable the logger
- `SHL_USE_CLI_PARSER` - enable the cli argument parsing
- `SHL_USE_NO_BUILD` - enable the no build tool
- `SHL_USE_HELPER` - enable some custom helper tools, like `TODO("not implemented yet")`
- `SHL_USE_DYN_ARRAY` - enable dynamic arrays, a list like tooling in c!

If you now include `#include "./build.h"` itself, you should be ready to start! Below is an example. this examples assumes there is a file `main.c` which is our target. When we are compiling this file (assume its `build.c`) with `gcc -o build build.c` and execute if afterwards with `./build`, it is doing multiple things as once.

1. check if build.c has changed, if so: recompile `build.c` and rerun itself
2. compile the target (here `main.c`)
3: autorun is on true, thereby: run the target.

See the Code below:

```c
// File: build.c

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#define SHL_USE_NO_BUILD

#include "./build.h"

int main() {
    auto_rebuild();

    BuildConfig build = (BuildConfig){
      .source   = "src/main.c",
      .output   = "out/main",
      .autorun  = true
    };

    if (!dispatch_build(&build)) return 1;
    info("We are finished! yeeey.");

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
