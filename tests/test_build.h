#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_cmd_create_and_push) {
    Cmd cmd = {0};
    QOL_TEST_EQ(cmd.len, 0, "initial length is 0");
    QOL_TEST_EQ(cmd.cap, 0, "initial capacity is 0");
    
    push(&cmd, "cc");
    QOL_TEST_EQ(cmd.len, 1, "length after push");
    QOL_TEST_STREQ(cmd.data[0], "cc", "first item is cc");
    
    push(&cmd, "main.c");
    QOL_TEST_EQ(cmd.len, 2, "length after second push");
    QOL_TEST_STREQ(cmd.data[1], "main.c", "second item is main.c");
    
    release(&cmd);
    QOL_TEST_EQ(cmd.len, 0, "length after release");
    QOL_TEST_EQ(cmd.cap, 0, "capacity after release");
}

QOL_TEST(test_default_c_build) {
    Cmd cmd = default_c_build("test.c", "test");
    
    QOL_TEST_TRUTHY(cmd.len > 0, "command has items");
    QOL_TEST_TRUTHY(cmd.data != NULL, "command data is allocated");
    
    // Check that it contains expected elements
    bool has_source = false;
    bool has_output = false;
    bool has_o_flag = false;
    
    for (size_t i = 0; i < cmd.len; i++) {
        if (cmd.data[i] && strcmp(cmd.data[i], "test.c") == 0) {
            has_source = true;
        }
        if (cmd.data[i] && strcmp(cmd.data[i], "test") == 0) {
            has_output = true;
        }
        if (cmd.data[i] && strcmp(cmd.data[i], "-o") == 0) {
            has_o_flag = true;
        }
    }
    
    QOL_TEST_TRUTHY(has_source, "command contains source file");
    QOL_TEST_TRUTHY(has_output, "command contains output file");
    QOL_TEST_TRUTHY(has_o_flag, "command contains -o flag");
    
    release(&cmd);
}

QOL_TEST(test_default_c_build_null_output) {
    Cmd cmd = default_c_build("test.c", NULL);
    
    QOL_TEST_TRUTHY(cmd.len > 0, "command has items");
    
    // Should auto-generate output name from source (test.c -> test)
    // Find the -o flag and check what comes after it
    bool has_output = false;
    for (size_t i = 0; i < cmd.len - 1; i++) {
        if (cmd.data[i] && strcmp(cmd.data[i], "-o") == 0 && cmd.data[i + 1]) {
            // Output should be after -o flag
            QOL_TEST_TRUTHY(cmd.data[i + 1] != NULL, "output exists after -o");
            has_output = true;
            break;
        }
    }
    
    QOL_TEST_TRUTHY(has_output, "auto-generated output name after -o");
    
    release(&cmd);
}

QOL_TEST(test_cmd_build_from_scratch) {
    Cmd cmd = {0};
    
    push(&cmd, "cc");
    push(&cmd, "-Wall");
    push(&cmd, "-Wextra");
    push(&cmd, "main.c");
    push(&cmd, "-o");
    push(&cmd, "main");
    
    QOL_TEST_EQ(cmd.len, 6, "command has 6 items");
    QOL_TEST_STREQ(cmd.data[0], "cc", "compiler is cc");
    QOL_TEST_STREQ(cmd.data[1], "-Wall", "first flag");
    QOL_TEST_STREQ(cmd.data[2], "-Wextra", "second flag");
    QOL_TEST_STREQ(cmd.data[3], "main.c", "source file");
    QOL_TEST_STREQ(cmd.data[4], "-o", "output flag");
    QOL_TEST_STREQ(cmd.data[5], "main", "output file");
    
    release(&cmd);
}

QOL_TEST(test_get_filename_no_ext) {
    char* name1 = get_filename_no_ext("test.c");
    QOL_TEST_STREQ(name1, "test", "simple filename");
    free(name1);
    
    char* name2 = get_filename_no_ext("/path/to/file.c");
    QOL_TEST_STREQ(name2, "file", "path with extension");
    free(name2);
    
    char* name3 = get_filename_no_ext("file.tar.gz");
    QOL_TEST_STREQ(name3, "file.tar", "multiple extensions");
    free(name3);
    
    char* name4 = get_filename_no_ext("noext");
    QOL_TEST_STREQ(name4, "noext", "no extension");
    free(name4);
}
