// Build system implementation: Compiles all examples and tests in parallel
// This file uses the build.h library to create a self-building build system.
// When build.c or build.h changes, the executable automatically rebuilds itself.

#define QOL_IMPLEMENTATION  // Include implementation code from build.h
#define QOL_STRIP_PREFIX    // Use shorter function names (e.g., 'run' instead of 'qol_run')
#include "./build.h"

// Command structure for building individual files (reused in loop)
Cmd cmd = {0};

// Array of process handles for parallel compilation of all examples
Procs procs = {0};

int main() {
    // Auto-rebuild this build system if build.c or build.h changes
    // If rebuild happens, the new executable replaces this process
    auto_rebuild_plus(__FILE__, "build.h");

    // Array of [source_file, output_binary] pairs for all examples and tests
    // Each example demonstrates a different feature of the build.h library
    char *examples[][2] = {
        {"examples/000_qol_logger.c", "out/000_logger"},
        {"examples/001_qol_cli_arg_parser.c", "out/001_argparser"},
        {"examples/002_qol_dynamic_array.c", "out/002_dyn_array"},
        {"examples/003_qol_helper.c", "out/003_helper"},
        {"examples/004_qol_file_utils.c", "out/004_file"},
        {"examples/005_qol_file_ops.c", "out/005_file_ops"},
        {"examples/006_qol_hashmap.c", "out/006_hashmap"},
        {"examples/007_qol_unittest.c", "out/007_unittest"},
        {"examples/008_qol_timer.c", "out/008_timer"},
        {"examples/010_qol_temp_alloc.c", "out/010_temp_alloc"},
        {"examples/011_qol_path_utils.c", "out/011_path_utils"},
        {"examples/012_qol_string_utils.c", "out/012_string_utils"},
        {"examples/901_qol_demo_calculator.c", "out/901_demo_calculator"},
        {"examples/902_qol_demo_pointer.c", "out/902_demo_pointer"},
        {"tests/unittests.c", "out/unittests"},
    };

    // Build all examples in parallel (async mode)
    // Each build command is added to the procs array for parallel execution
    // If any build fails immediately (sync check), exit early
    for (size_t i = 0; i < ARRAY_LEN(examples); i++) {
        // Create default C build command: cc source.c -o output
        cmd = default_c_build(examples[i][0], examples[i][1]);
        
        // Run build command asynchronously, adding process handle to procs array
        // If run() returns false, it means the command failed synchronously (before async start)
        // In that case, exit early rather than continuing with broken builds
        if (!run(&cmd, .procs = &procs)) return EXIT_SUCCESS;
    }

    // Wait for all parallel builds to complete and check if all succeeded
    // Returns false if any build failed, true if all builds succeeded
    if (!procs_wait(&procs)) return EXIT_FAILURE;

    // All builds completed successfully
    return EXIT_SUCCESS;
}
