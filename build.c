#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "./build.h"

Cmd cmd = {0};
Procs procs = {0};

int main() {
    auto_rebuild_plus(__FILE__, "build.h");
    init_logger(LOG_INFO, false, true);

    // Read all .c files from examples/ and compile them into out/

    const char* src_folder = "examples";
    const char* out_folder = "out";

    String contents = {0};
    if (!read_dir(src_folder, &contents)) return EXIT_FAILURE;

    for (size_t i = 0; i < contents.len; i++) {
        const char* src_path = contents.data[i];
        
        if (!str_ends_with(src_path, ".c")) continue;
        const char* temp = str_replace(src_path, ".c", "");
        const char* new_path = str_replace(temp, src_folder, out_folder);

        if (!temp || !new_path) continue;
        cmd = default_c_build(src_path, new_path);
        if (is_windows && str_contains(src_path, "013_qol_thread_safety")) push(&cmd, "-pthread");
        if (!run(&cmd, .procs = &procs)) return EXIT_SUCCESS;
    }

    if (!procs_wait(&procs)) return EXIT_FAILURE;

    // Build unittests
    cmd = default_c_build("tests/unittests.c", "out/unittests");
    if (!run(&cmd)) return EXIT_SUCCESS;

    return EXIT_SUCCESS;
}
