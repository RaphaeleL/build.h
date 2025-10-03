# build.h

A single-header **quality-of-life (QoL) utility library for C**. It provides logging, CLI argument parsing, dynamic arrays, and lightweight build-system helpers in a portable way (Linux, macOS, Windows).

## Getting Started

Just drop `build.h` into your project.

```bash
wget https://raw.githubusercontent.com/RaphaeleL/build.h/refs/heads/main/build.h
```

There are several `define` options, which control the behaviour of `build.h`:

- `SHL_USE_LOGGER` - enable the logger
- `SHL_USE_CLI_PARSER` - enable the cli argument parsing
- `SHL_USE_NO_BUILD` - enable the no build tool
- `SHL_USE_HELPER` - enable some custom helper dev tools
- `SHL_USE_DYN_ARRAY` - enable dynamic arrays

Since it's just an header file, you need to define `SHL_IMPLEMENTATION` in **one** of your source files before including `build.h`, to enable the implementation inside of the header.

If you now include `#include "./build.h"` itself, you should be ready to start! Below is an example. this examples assumes there is a file `main.c` which is our target. When we are compiling this file (assume its `build.c`) with `gcc -o build build.c` and execute if afterwards with `./build`, it is doing multiple things as once.

1. check if build.c has changed, if so: recompile `build.c` and rerun itself
2. compile the target (here `main.c`)
3. autorun is on true, thereby: run the target.

Checkout the `examples/` directory with many different example usages. The Code below is just showing the No Build Tooling!

```c
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#define SHL_USE_NO_BUILD

#include "./build.h"

int main() {
    auto_rebuild();

    BuildConfig build = default_build_config();
    build.source = "src/main.c";
    build.output = "out/main";

    if (!dispatch_build(&build)) return 1;
    info("We are finished! yeeey.");

    return 0;
}
```

### How to create a valid BuildConfig

A `BuildConfig` is a struct which holds all the information needed to build your target. Let's assume we want to re-create follwoing command:

```bash
gcc -Wall -O2 -std=c11 -DDEBUG=1 -Iinclude main.c util.c -o prog -L/usr/local/lib -lm -pthread
```

This can be done like this:

```c
SHL_BuildConfig cfg = {
    .source = "main.c"
    .output = "prog",
    .compiler = "gcc",
    .compiler_flags = "-Wall -O2 -std=c11",
    .linker_flags = "-pthread",
    .include_dirs = "include",
    .libraries = "m",
    .library_dirs = "/usr/local/lib",
    .defines = "DEBUG=1",
    .autorun = true
};
```

Alternatively you firstly create an empty default `BuildConfig` and then add all the options you need:

```c
SHL_BuildConfig cfg = default_build_config();
cfg.sources = "main.c util.c";
cfg.output = "prog";
cfg.compiler_flags = "-Wall -O2 -std=c11";
cfg.linker_flags = "-pthread";
cfg.include_dirs = "include";
cfg.libraries = "m";
cfg.library_dirs = "/usr/local/lib";
cfg.defines = "DEBUG=1";
```

##  Roadmap

Since each Tool is independent and you can just enable the ones you need. The roadmap below shows what is already implemented and what is planned. They will all be included in the same single-header file. In the future this might be split into multiple files, but for now I want to keep it simple.

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
