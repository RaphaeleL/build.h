#define QOL_IMPLEMENTATION
#define QOL_USE_LOGGER
#define QOL_USE_FILE_OPS
#include "../build.h"
#include <string.h>

int main() {
    qol_info("=== File Operations Test ===\n\n");

    // Test 1: Directory creation
    qol_info("\n--- Test 1: Directory Creation ---\n");
    if (qol_mkdir_if_not_exists("test_dir")) {
        qol_info("Directory 'test_dir' created or already exists\n");
    }

    if (qol_mkdir_if_not_exists("test_dir/sub_dir")) {
        qol_info("Subdirectory 'test_dir/sub_dir' created\n");
    }

    // Test 2: File writing
    qol_info("\n--- Test 2: File Writing ---\n");
    const char *test_content = "Hello, World!\nThis is a test file.\nLine 3\n";
    if (qol_write_file("test_file.txt", test_content, strlen(test_content))) {
        qol_info("File written successfully\n");
    }

    // Test 3: File type detection
    qol_info("\n--- Test 3: File Type Detection ---\n");
    qol_info("test_file.txt has extension: %s\n", qol_get_file_type("test_file.txt"));
    qol_info("file.no_ext has extension: %s\n", qol_get_file_type("file.no_ext"));
    qol_info("noextension has extension: %s\n", qol_get_file_type("noextension"));

    // Test 4: File reading
    qol_info("\n--- Test 4: File Reading ---\n");
    QOL_String content = {0};
    if (qol_read_file("test_file.txt", &content)) {
        qol_info("File read successfully (%zu lines):\n", content.len);
        for (size_t i = 0; i < content.len; i++) {
            qol_info("  Line %zu: %s\n", i + 1, content.data[i]);
        }
        // Free the dynamic array and all strings
        qol_release_string(&content);
    }

    // Test 5: File copying
    qol_info("\n--- Test 5: File Copying ---\n");
    if (qol_copy_file("test_file.txt", "test_file_copy.txt")) {
        qol_info("File copied successfully\n");
    }

    // Read the copied file to verify
    QOL_String copied_content = {0};
    if (qol_read_file("test_file_copy.txt", &copied_content)) {
        qol_info("Copied file verified (%zu lines)\n", copied_content.len);
        qol_release_string(&copied_content);
    }

    // Test 6: Directory listing
    qol_info("\n--- Test 6: Directory Listing ---\n");
    QOL_String dir_contents = {0};
    if (qol_read_dir(".", &dir_contents)) {
        qol_info("Directory contains %zu entries:\n", dir_contents.len);
        for (size_t i = 0; i < dir_contents.len; i++) {
            qol_info("  %s\n", dir_contents.data[i]);
        }
        qol_release_string(&dir_contents);
    }

    // Test 7: Recursive directory copy
    qol_info("\n--- Test 7: Recursive Directory Copy ---\n");
    
    // Create some test files in the source directory
    const char *file1_content = "File 1 content\n";
    const char *file2_content = "File 2 content\n";
    qol_write_file("test_dir/file1.txt", file1_content, strlen(file1_content));
    qol_write_file("test_dir/file2.txt", file2_content, strlen(file2_content));
    qol_write_file("test_dir/sub_dir/file3.txt", "File 3 content\n", 15);

    if (qol_copy_dir_rec("test_dir", "test_dir_copy")) {
        qol_info("Directory copied recursively\n");
        
        // List the copied directory to verify
        qol_info("\nContents of copied directory:\n");
        QOL_String copied_dir_contents = {0};
        if (qol_read_dir("test_dir_copy", &copied_dir_contents)) {
            for (size_t i = 0; i < copied_dir_contents.len; i++) {
                qol_info("  %s\n", copied_dir_contents.data[i]);
            }
            qol_release_string(&copied_dir_contents);
        }
    }

    // Test 8: File deletion
    qol_info("\n--- Test 8: File Deletion ---\n");
    if (qol_delete_file("test_file_copy.txt")) {
        qol_info("File deleted successfully\n");
    }

    // Verify deletion by trying to read it
    QOL_String deleted_content = {0};
    if (!qol_read_file("test_file_copy.txt", &deleted_content)) {
        qol_info("Confirmed file does not exist after deletion\n");
    }

    // Test 9: Directory deletion
    qol_info("\n--- Test 9: Directory Deletion ---\n");
    if (qol_delete_dir("test_dir")) {
        qol_info("Directory 'test_dir' deleted successfully\n");
    }

    qol_info("\n=== All Tests Completed ===\n");

    return EXIT_SUCCESS;
}

