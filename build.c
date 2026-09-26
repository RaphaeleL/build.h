#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "./build.h"

int main() {
    init_logger(.level=LOG_INFO, .time=true, .color=!true, .time_color=!true);
    auto_rebuild_plus(__FILE__, "build.h");

    const char* src_folder = "examples";
    const char* out_folder = "out";

    String contents = {0};
    if (!get_files_in_dir(src_folder, &contents)) return EXIT_FAILURE;

    list(Cmd) builds = {0};
    list(char*) owned = {0};

    for (size_t i = 0; i < contents.len; i++) {
        const char* src_path = contents.data[i];
        if (!str_ends_with(src_path, ".c")) continue;

        const char* temp = str_replace(src_path, ".c", "");
        const char* new_path = str_replace(temp, src_folder, out_folder);
        if (!temp || !new_path) {
            if (temp) free((void*)temp);
            if (new_path) free((void*)new_path);
            continue;
        }

        Cmd cmd = default_c_build(src_path, new_path);
        if (is_windows && str_contains(src_path, "013_qol_thread_safety")) push(&cmd, "-pthread");
        push(&builds, cmd);
        push(&owned, (char*)temp);
        push(&owned, (char*)new_path);
    }

    if (builds.len > 0 && !run_parallel(builds.data, builds.len)) {
        release_string(&contents);
        for (size_t i = 0; i < owned.len; i++) free(owned.data[i]);
        release(&builds);
        release(&owned);
        return EXIT_FAILURE;
    }

    // Every `Cmd` still points at the strings owned by `contents` (the source
    // paths) and by `owned` (the output paths), so they must outlive the run.
    release_string(&contents);

    for (size_t i = 0; i < owned.len; i++) free(owned.data[i]);
    release(&builds);
    release(&owned);

    mkdir_if_not_exists("libs");

    Cmd obj = default_c_build_extended("build.h", "libs/build.o",
        (const char*[]){"-x", "c", "-D", "QOL_IMPLEMENTATION", "-c"}, 5, "cc");
    if (!run_always(&obj)) return EXIT_FAILURE;

    Cmd static_lib = {0};
    push(&static_lib, "ar", "rcs", "libs/build.a", "libs/build.o");
    if (!run_always(&static_lib)) return EXIT_FAILURE;

#if defined(WINDOWS)
    const char* shared_out = "libs/build.dll";
#else
    const char* shared_out = "libs/build.so";
#endif
    Cmd shared_lib = default_c_build_extended("build.h", shared_out,
        (const char*[]){"-fPIC", "-shared", "-x", "c", "-D", "QOL_IMPLEMENTATION"}, 6, "cc");
    if (!run_always(&shared_lib)) return EXIT_FAILURE;

    delete_file("libs/build.o");

    Cmd unittest = default_c_build("tests/unittests.c", "out/unittests");
    if (!run(&unittest)) return EXIT_SUCCESS;

    return EXIT_SUCCESS;
}
