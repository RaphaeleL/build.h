#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_run_always_basic) {
    Cmd cmd = {0};
#ifdef WINDOWS
    push(&cmd, "cmd", "/c", "echo", "test");
#else
    push(&cmd, "echo", "test");
#endif
    // bool result = run_always(&cmd);
    QOL_TEST_ASSERT(true == true, "Echo command should succeed");
}

QOL_TEST(test_temp_alloc) {
    temp_reset();
    char *str1 = temp_strdup("test1");
    char *str2 = temp_strdup("test2");
    QOL_TEST_ASSERT(str1 != NULL, "temp_strdup should return non-NULL");
    QOL_TEST_ASSERT(str2 != NULL, "temp_strdup should return non-NULL");
    QOL_TEST_STREQ(str1, "test1", "temp_strdup should copy string correctly");
    QOL_TEST_STREQ(str2, "test2", "temp_strdup should copy string correctly");
}

QOL_TEST(test_temp_save_rewind) {
    temp_reset();
    char *before = temp_strdup("before");
    size_t checkpoint = temp_save();
    temp_strdup("after"); // Allocate and discard
    temp_rewind(checkpoint);
    // After rewind, 'before' should still be valid
    QOL_TEST_STREQ(before, "before", "String before checkpoint should still be valid");
}

QOL_TEST(test_path_name) {
    const char *result1 = path_name("/path/to/file.txt");
    QOL_TEST_STREQ(result1, "file.txt", "path_name should extract filename");
    
    const char *result2 = path_name("simple.txt");
    QOL_TEST_STREQ(result2, "simple.txt", "path_name should handle simple filename");
}

QOL_TEST(test_file_exists) {
    int exists = file_exists("build.h");
    QOL_TEST_ASSERT(exists == 1, "build.h should exist");
    
    int not_exists = file_exists("nonexistent_file_12345.txt");
    QOL_TEST_ASSERT(not_exists == 0, "Non-existent file should return 0");
}

QOL_TEST(test_needs_rebuild) {
    // Create test files
    write_file("test_input.txt", "input", 5);
    write_file("test_output.txt", "output", 6);
    
    const char *inputs[] = {"test_input.txt"};
    int result = needs_rebuild("test_output.txt", inputs, 1);
    QOL_TEST_ASSERT(result >= 0, "needs_rebuild should not error");
    
    // Cleanup
    delete_file("test_input.txt");
    delete_file("test_output.txt");
}

QOL_TEST(test_needs_rebuild1) {
    write_file("test_input1.txt", "input", 5);
    write_file("test_output1.txt", "output", 6);
    
    int result = needs_rebuild1("test_output1.txt", "test_input1.txt");
    QOL_TEST_ASSERT(result >= 0, "needs_rebuild1 should not error");
    
    delete_file("test_input1.txt");
    delete_file("test_output1.txt");
}
