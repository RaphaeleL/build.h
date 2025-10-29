#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

static const char* shl_temp_dir_path(void) {
    static char path[256];
    snprintf(path, sizeof(path), "/tmp/shl_tests_%ld", (long)time(NULL));
    return path;
}

SHL_TEST(test_file_ops_mkdir_write_read_delete) {
    const char* dir = shl_temp_dir_path();
    SHL_TEST_TRUTHY(mkdir_if_not_exists(dir), "mkdir_if_not_exists works");

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir, "sample.txt");

    const char* content = "hello\nworld\n";
    SHL_TEST_TRUTHY(write_file(file_path, content, strlen(content)), "write_file succeeds");

    SHL_String lines = {0};
    SHL_TEST_TRUTHY(read_file(file_path, &lines), "read_file succeeds");
    SHL_TEST_EQ(lines.len, 2, "two lines read");
    SHL_TEST_STREQ(lines.data[0], "hello", "first line matches");
    SHL_TEST_STREQ(lines.data[1], "world", "second line matches");
    release_string(&lines);

    SHL_TEST_STREQ(get_file_type(file_path), "txt", "file extension detected");

    SHL_TEST_TRUTHY(delete_file(file_path), "delete_file succeeds");
}

SHL_TEST(test_file_get_filename_no_ext) {
    const char* path = "/var/tmp/foo/bar/baz.tar.c";
    char* name = get_filename_no_ext(path);
    SHL_TEST_STREQ(name, "baz.tar", "filename without last extension");
    free(name);
}

