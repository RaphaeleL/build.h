/*
 * ===========================================================================
 * 012_qol_string_utils.c
 *
 * Example usage for string utilities
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
    info("Example 1: String starts with\n");
    const char *str1 = "Hello, World!";
    info("str_starts_with(\"%s\", \"Hello\") = %s\n", str1, str_starts_with(str1, "Hello") ? "true" : "false");
    info("str_starts_with(\"%s\", \"World\") = %s\n", str1, str_starts_with(str1, "World") ? "true" : "false");

    info("\n");
    info("Example 2: String ends with\n");
    info("str_ends_with(\"%s\", \"World!\") = %s\n", str1, str_ends_with(str1, "World!") ? "true" : "false");
    info("str_ends_with(\"%s\", \"Hello\") = %s\n", str1, str_ends_with(str1, "Hello") ? "true" : "false");

    info("\n");
    info("Example 3: String trim (in-place)\n");
    char str2[] = "   Hello, World!   ";
    info("Before trim: \"%s\"\n", str2);
    str_trim(str2);
    info("After trim: \"%s\"\n", str2);

    info("\n");
    info("Example 4: String ltrim and rtrim\n");
    char str3[] = "   Left trim   ";
    char str4[] = "   Right trim   ";
    info("Before ltrim: \"%s\"\n", str3);
    str_ltrim(str3);
    info("After ltrim: \"%s\"\n", str3);
    info("Before rtrim: \"%s\"\n", str4);
    str_rtrim(str4);
    info("After rtrim: \"%s\"\n", str4);

    info("\n");
    info("Example 5: String replace\n");
    const char *str5 = "Hello, World! Hello, World!";
    char *replaced = str_replace(str5, "World", "Universe");
    if (replaced) {
        info("Original: \"%s\"\n", str5);
        info("Replaced: \"%s\"\n", replaced);
        free(replaced);
    }

    info("\n");
    info("Example 6: String split\n");
    const char *str6 = "apple,banana,cherry,date";
    String parts = {0};
    if (str_split(str6, ',', &parts)) {
        info("Split \"%s\" by ',':\n", str6);
        for (size_t i = 0; i < parts.len; i++) {
            info("  [%zu] = \"%s\"\n", i, parts.data[i]);
        }
        release_string(&parts);
    }

    info("\n");
    info("Example 7: String join\n");
    String fruits = {0};
    push(&fruits, strdup("apple"));
    push(&fruits, strdup("banana"));
    push(&fruits, strdup("cherry"));
    char *joined = str_join(&fruits, ",");
    if (joined) {
        info("Joined: \"%s\"\n", joined);
        free(joined);
    }
    release_string(&fruits);

    info("\n");
    info("Example 8: String contains\n");
    const char *str7 = "The quick brown fox";
    info("str_contains(\"%s\", \"brown\") = %s\n", str7, str_contains(str7, "brown") ? "true" : "false");
    info("str_contains(\"%s\", \"red\") = %s\n", str7, str_contains(str7, "red") ? "true" : "false");

    info("\n");
    info("Example 9: Case-insensitive compare\n");
    const char *str8 = "Hello";
    const char *str9 = "HELLO";
    const char *str10 = "World";
    info("str_icmp(\"%s\", \"%s\") = %d\n", str8, str9, str_icmp(str8, str9));
    info("str_icmp(\"%s\", \"%s\") = %d\n", str8, str10, str_icmp(str8, str10));

    return EXIT_SUCCESS;
}
