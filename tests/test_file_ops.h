#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

static const char* qol_temp_dir_path(void) {
    static char path[256];
    snprintf(path, sizeof(path), "/tmp/qol_tests_%ld", (long)time(NULL));
    return path;
}

QOL_TEST(test_file_ops_mkdir_write_read_delete) {
    const char* dir = qol_temp_dir_path();
    QOL_TEST_TRUTHY(mkdir_if_not_exists(dir), "mkdir_if_not_exists works");

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir, "sample.txt");

    const char* content = "hello\nworld\n";
    QOL_TEST_TRUTHY(write_file(file_path, content, strlen(content)), "write_file succeeds");

    QOL_String lines = {0};
    QOL_TEST_TRUTHY(read_file(file_path, &lines), "read_file succeeds");
    QOL_TEST_EQ(lines.len, 2, "two lines read");
    QOL_TEST_STREQ(lines.data[0], "hello", "first line matches");
    QOL_TEST_STREQ(lines.data[1], "world", "second line matches");
    release_string(&lines);

    QOL_TEST_STREQ(get_file_type(file_path), "txt", "file extension detected");

    QOL_TEST_TRUTHY(delete_file(file_path), "delete_file succeeds");
}

QOL_TEST(test_file_get_filename_no_ext) {
    const char* path = "/var/tmp/foo/bar/baz.tar.c";
    char* name = get_filename_no_ext(path);
    QOL_TEST_STREQ(name, "baz.tar", "filename without last extension");
    free(name);
}

