/*
 * ===========================================================================
 * 010_shl_temp_alloc.c
 *
 * Example usage for temporary allocator
 *
 * Created: 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

int main() {
    info("Example 1: Basic temp allocation\n");
    char *str1 = temp_strdup("Hello");
    char *str2 = temp_strdup("World");
    info("str1: %s, str2: %s\n", str1, str2);

    info("Example 2: temp_sprintf\n");
    char *formatted = temp_sprintf("Value: %d, Name: %s", 42, "test");
    info("Formatted: %s\n", formatted);

    info("Example 3: Save and rewind\n");
    size_t checkpoint = temp_save();
    char *after_checkpoint = temp_strdup("This will be freed");
    info("After checkpoint: %s", after_checkpoint);
    temp_rewind(checkpoint);
    info("After rewind, memory marked as reusable (not erased!)\n");
    warn("WARNING: after_checkpoint pointer is now INVALID!\n");
    warn("The data might still appear to be there, but don't use it!\n");

    info("Example 4: Reset all\n");
    char *before_reset = temp_strdup("Before reset");
    info("Before reset: %s\n", before_reset);
    temp_reset();
    info("After reset, all temp memory marked as reusable\n");
    warn("WARNING: All previous pointers (str1, str2, formatted, before_reset) are now INVALID!\n");
    
    info("Example 5: Memory reuse demonstration\n");
    char *new_str = temp_strdup("NEW DATA OVERWRITES OLD");
    info("New allocation: %s\n", new_str);
    info("Old str1 (INVALID pointer - undefined behavior!): %s\n", str1);
    warn("This demonstrates why you shouldn't use pointers after reset!\n");
    warn("The arena allocator doesn't erase data - it just marks memory as reusable.\n");
    warn("Old data remains until overwritten by new allocations.\n");
    return EXIT_SUCCESS;
}
