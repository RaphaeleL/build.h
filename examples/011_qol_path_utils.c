/*
 * ===========================================================================
 * 011_qol_path_utils.c
 *
 * Example usage for path utilities
 *
 * Created: 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

int main() {
    info("Example 1: Extract filename from path\n");
    const char *path1 = "/path/to/file.txt";
    const char *path2 = "simple.txt";
    info("path_name(\"%s\") = %s\n", path1, path_name(path1));
    info("path_name(\"%s\") = %s\n", path2, path_name(path2));

    info("Example 2: Get current directory\n");
    const char *cwd = get_current_dir_temp();
    if (cwd) {
        info("Current directory: %s\n", cwd);
    }

    info("Example 3: Check if file exists\n");
    int exists = file_exists("build.h");
    if (exists == 1) {
        info("build.h exists\n");
    } else if (exists == 0) {
        info("build.h does not exist\n");
    }

    info("Example 4: Rename file\n");
    write_file("test_rename.txt", "test content", 12);
    if (rename("test_rename.txt", "test_renamed.txt")) {
        info("File renamed successfully\n");
        delete_file("test_renamed.txt");
    }

    info("Example 5: Change directory\n");
    if (mkdir_if_not_exists("test_dir")) {
        if (set_current_dir("test_dir")) {
            const char *new_cwd = get_current_dir_temp();
            info("Changed to: %s\n", new_cwd);
            set_current_dir("..");
        }
        delete_dir("test_dir");
    }

    return EXIT_SUCCESS;
}
