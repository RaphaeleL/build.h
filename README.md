# Single Header Library (SHL)

This repository provides a collection of **Single Header Libraries (SHL)** written in **C**.

A Single Header Library is a **standalone `.h` file** that can be easily dropped into any C project without additional build steps, dependencies, or complex setup.
The goal of this project is to deliver **lightweight**, **portable**, and **easy-to-use** libraries for C developers.

## Features

* **Header-only**: just `#include` and start coding.
* **Minimal dependencies**: no external libraries required.
* **Portable**: designed to work across Linux, macOS, and Windows.
* **Well-documented**: each SHL comes with usage examples.

## Build the Examples

Just compile the `build.c` custom build tool and execute it. Every `example/` will be compiled.

```bash
gcc build.c -o build
./build
```

## Usage in your own Project

1. Copy the desired `.h` file into your project.
2. Include it in your source file:

```c
#define SHL_IMPLEMENTATION
#include "shl_example.h"

int main(void) {
    shl_hello();
    return 0;
}
```

3. Compile as usual:

```bash
gcc main.c -o main
./main
```

# License

This project is licensed under the MIT License.
You are free to use, modify, and distribute the code with proper attribution.
