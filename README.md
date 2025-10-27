# build.h

A single-header **quality-of-life (QoL) utility library for C**. It provides logging, CLI argument parsing, dynamic arrays, and lightweight build-system helpers in a portable way (Linux, macOS, Windows).

## Getting Started

Just drop `build.h` into your project.

```bash
wget https://raw.githubusercontent.com/RaphaeleL/build.h/refs/heads/main/build.h
```

Since it's just an header file, you need to define `SHL_IMPLEMENTATION` in **one** of your source files before including `build.h`, to enable the implementation inside of the header.

If you now include `#include "./build.h"` itself, you should be ready to start! Now you can either

1. Recreate a Build System with `build.h` which is Building Projects. 
2. Use the utilities `build.h` provides to use certain utilities like logging, CLI argument parsing, dynamic arrays, etc.

Below is an example. This examples assumes there is a file `main.c` which is our target. When we are compiling our Implementation of `build.h` (assume its `build.c`) with `gcc -o build build.c` and execute if afterwards with `./build`, it is doing multiple things as once.

1. check if build.c has changed, if so: recompile `build.c` and rerun itself
2. compile the target (here `main.c`)

```c
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX

#include "./build.h"

int main(int argc, char **argv) {
    init_logger(LOG_INFO, true, true);
    auto_rebuild("build.c");

    BuildConfig build = default_build_config("main.c", "main");

    if (!run(&build)) return 1;
    info("We are finished! yeeey.");

    return 0;
}
```

As mentioned above, you don't have to use `build.h` as a build tool. You can also just use the utilities it provides. Here is an example (assume its `demo.c`) of rebuilding `demo.c` and use the logger:

```c
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "./build.h"

int main() {
    auto_rebuild_plus("demo.c", "build.h");

    info("Hello, World!");

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
    .source = "main.c",
    .output = "prog",
    .compiler = "gcc",
    .compiler_flags = "-Wall -O2 -std=c11",
    .linker_flags = "-pthread",
    .include_dirs = "include",
    .libraries = "m",
    .library_dirs = "/usr/local/lib",
    .defines = "DEBUG=1"
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

### How to create a valid SystemConfig

Beside a Build Configuration, there is also a System Configuration. This is used to specify a CLI Command, for example an `ls -al` Command. 

##  Roadmap

The roadmap below shows what is already implemented and what is planned. They will all be included in the same single-header file. In the future this might be split into multiple files, but for now I want to keep it simple.

- [x] Logger
- [x] No Build
- [x] Dynamic Arrays
- [x] Helper Macros
- [x] CLI Argument Parser
- [x] File Operations
- [x] Hashmap
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
