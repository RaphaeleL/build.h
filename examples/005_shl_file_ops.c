#define SHL_IMPLEMENTATION
#define SHL_USE_LOGGER
#define SHL_USE_FILE_OPS
#include "../build.h"
#include <string.h>

int main() {
    shl_info("=== File Operations Test ===\n\n");

    // Test 1: Directory creation
    shl_info("\n--- Test 1: Directory Creation ---\n");
    if (shl_mkdir_if_not_exists("test_dir")) {
        shl_info("Directory 'test_dir' created or already exists\n");
    }

    if (shl_mkdir_if_not_exists("test_dir/sub_dir")) {
        shl_info("Subdirectory 'test_dir/sub_dir' created\n");
    }

    // Test 2: File writing
    shl_info("\n--- Test 2: File Writing ---\n");
    const char *test_content = "Hello, World!\nThis is a test file.\nLine 3\n";
    if (shl_write_file("test_file.txt", test_content, strlen(test_content))) {
        shl_info("File written successfully\n");
    }

    // Test 3: File type detection
    shl_info("\n--- Test 3: File Type Detection ---\n");
    shl_info("test_file.txt has extension: %s\n", shl_get_file_type("test_file.txt"));
    shl_info("file.no_ext has extension: %s\n", shl_get_file_type("file.no_ext"));
    shl_info("noextension has extension: %s\n", shl_get_file_type("noextension"));

    // Test 4: File reading
    shl_info("\n--- Test 4: File Reading ---\n");
    SHL_String content = {0};
    if (shl_read_file("test_file.txt", &content)) {
        shl_info("File read successfully (%zu lines):\n", content.len);
        for (size_t i = 0; i < content.len; i++) {
            shl_info("  Line %zu: %s\n", i + 1, content.data[i]);
        }
        // Free the dynamic array and all strings
        shl_release_string(&content);
    }

    // Test 5: File copying
    shl_info("\n--- Test 5: File Copying ---\n");
    if (shl_copy_file("test_file.txt", "test_file_copy.txt")) {
        shl_info("File copied successfully\n");
    }

    // Read the copied file to verify
    SHL_String copied_content = {0};
    if (shl_read_file("test_file_copy.txt", &copied_content)) {
        shl_info("Copied file verified (%zu lines)\n", copied_content.len);
        shl_release_string(&copied_content);
    }

    // Test 6: Directory listing
    shl_info("\n--- Test 6: Directory Listing ---\n");
    shl_read_dir(".", NULL);

    // Test 7: Recursive directory copy
    shl_info("\n--- Test 7: Recursive Directory Copy ---\n");
    
    // Create some test files in the source directory
    const char *file1_content = "File 1 content\n";
    const char *file2_content = "File 2 content\n";
    shl_write_file("test_dir/file1.txt", file1_content, strlen(file1_content));
    shl_write_file("test_dir/file2.txt", file2_content, strlen(file2_content));
    shl_write_file("test_dir/sub_dir/file3.txt", "File 3 content\n", 15);

    if (shl_copy_dir_rec("test_dir", "test_dir_copy")) {
        shl_info("Directory copied recursively\n");
        
        // List the copied directory to verify
        shl_info("\nContents of copied directory:\n");
        shl_read_dir("test_dir_copy", NULL);
    }

    // Test 8: File deletion
    shl_info("\n--- Test 8: File Deletion ---\n");
    if (shl_delete_file("test_file_copy.txt")) {
        shl_info("File deleted successfully\n");
    }

    // Verify deletion by trying to read it
    SHL_String deleted_content = {0};
    if (!shl_read_file("test_file_copy.txt", &deleted_content)) {
        shl_info("Confirmed file does not exist after deletion\n");
    }

    shl_info("\n=== All Tests Completed ===\n");

    return EXIT_SUCCESS;
}

