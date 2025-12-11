#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_str_starts_with) {
    QOL_TEST_TRUTHY(str_starts_with("Hello, World!", "Hello"), "starts with prefix");
    QOL_TEST_FALSY(str_starts_with("Hello, World!", "World"), "does not start with suffix");
    QOL_TEST_TRUTHY(str_starts_with("test", "test"), "starts with itself");
    QOL_TEST_FALSY(str_starts_with("short", "very long prefix"), "prefix longer than string");
    QOL_TEST_FALSY(str_starts_with(NULL, "test"), "NULL string returns false");
    QOL_TEST_FALSY(str_starts_with("test", NULL), "NULL prefix returns false");
}

QOL_TEST(test_str_ends_with) {
    QOL_TEST_TRUTHY(str_ends_with("Hello, World!", "World!"), "ends with suffix");
    QOL_TEST_FALSY(str_ends_with("Hello, World!", "Hello"), "does not end with prefix");
    QOL_TEST_TRUTHY(str_ends_with("test", "test"), "ends with itself");
    QOL_TEST_FALSY(str_ends_with("short", "very long suffix"), "suffix longer than string");
    QOL_TEST_FALSY(str_ends_with(NULL, "test"), "NULL string returns false");
    QOL_TEST_FALSY(str_ends_with("test", NULL), "NULL suffix returns false");
}

QOL_TEST(test_str_trim) {
    char str1[] = "   hello   ";
    str_trim(str1);
    QOL_TEST_STREQ(str1, "hello", "trim removes both sides");

    char str2[] = "hello";
    str_trim(str2);
    QOL_TEST_STREQ(str2, "hello", "trim on already trimmed string");

    char str3[] = "   ";
    str_trim(str3);
    QOL_TEST_STREQ(str3, "", "trim whitespace-only string");

    char str4[] = "\t\n\r hello \t\n\r";
    str_trim(str4);
    QOL_TEST_STREQ(str4, "hello", "trim various whitespace characters");
}

QOL_TEST(test_str_ltrim) {
    char str1[] = "   hello";
    str_ltrim(str1);
    QOL_TEST_STREQ(str1, "hello", "ltrim removes leading whitespace");

    char str2[] = "hello   ";
    str_ltrim(str2);
    QOL_TEST_STREQ(str2, "hello   ", "ltrim does not remove trailing whitespace");
}

QOL_TEST(test_str_rtrim) {
    char str1[] = "hello   ";
    str_rtrim(str1);
    QOL_TEST_STREQ(str1, "hello", "rtrim removes trailing whitespace");

    char str2[] = "   hello";
    str_rtrim(str2);
    QOL_TEST_STREQ(str2, "   hello", "rtrim does not remove leading whitespace");
}

QOL_TEST(test_str_replace) {
    const char *str = "Hello, World! Hello, World!";
    char *result = str_replace(str, "World", "Universe");
    QOL_TEST_ASSERT(result != NULL, "replacement succeeds");
    QOL_TEST_STREQ(result, "Hello, Universe! Hello, Universe!", "replaces all occurrences");
    free(result);

    char *result2 = str_replace("test", "x", "y");
    QOL_TEST_ASSERT(result2 != NULL, "replacement with no match succeeds");
    QOL_TEST_STREQ(result2, "test", "no match returns original");
    free(result2);

    char *result3 = str_replace("aaa", "a", "b");
    QOL_TEST_ASSERT(result3 != NULL, "replacement succeeds");
    QOL_TEST_STREQ(result3, "bbb", "replaces all single characters");
    free(result3);
}

QOL_TEST(test_str_split) {
    const char *str = "apple,banana,cherry";
    String parts = {0};
    QOL_TEST_TRUTHY(str_split(str, ',', &parts), "split succeeds");
    QOL_TEST_EQ(parts.len, 3, "split creates correct number of parts");
    QOL_TEST_STREQ(parts.data[0], "apple", "first part correct");
    QOL_TEST_STREQ(parts.data[1], "banana", "second part correct");
    QOL_TEST_STREQ(parts.data[2], "cherry", "third part correct");
    release_string(&parts);

    String parts2 = {0};
    QOL_TEST_TRUTHY(str_split("a,b,c,", ',', &parts2), "split with trailing delimiter succeeds");
    QOL_TEST_EQ(parts2.len, 4, "trailing delimiter creates empty string");
    QOL_TEST_STREQ(parts2.data[3], "", "trailing empty string");
    release_string(&parts2);
}

QOL_TEST(test_str_join) {
    String parts = {0};
    push(&parts, strdup("apple"));
    push(&parts, strdup("banana"));
    push(&parts, strdup("cherry"));
    char *joined = str_join(&parts, ", ");
    QOL_TEST_ASSERT(joined != NULL, "join succeeds");
    QOL_TEST_STREQ(joined, "apple, banana, cherry", "join creates correct string");
    free(joined);
    release_string(&parts);

    String empty = {0};
    char *joined2 = str_join(&empty, ", ");
    QOL_TEST_ASSERT(joined2 != NULL, "join empty array succeeds");
    QOL_TEST_STREQ(joined2, "", "join empty array returns empty string");
    free(joined2);
}

QOL_TEST(test_str_contains) {
    QOL_TEST_TRUTHY(str_contains("Hello, World!", "World"), "contains substring");
    QOL_TEST_FALSY(str_contains("Hello, World!", "Universe"), "does not contain substring");
    QOL_TEST_TRUTHY(str_contains("test", "test"), "contains itself");
    QOL_TEST_FALSY(str_contains("short", "very long substring"), "substring longer than string");
    QOL_TEST_FALSY(str_contains(NULL, "test"), "NULL string returns false");
    QOL_TEST_FALSY(str_contains("test", NULL), "NULL substring returns false");
}

QOL_TEST(test_str_icmp) {
    QOL_TEST_EQ(str_icmp("Hello", "HELLO"), 0, "case-insensitive compare equal");
    QOL_TEST_EQ(str_icmp("hello", "HELLO"), 0, "case-insensitive compare equal lowercase");
    QOL_TEST_NEQ(str_icmp("Hello", "World"), 0, "case-insensitive compare different strings");
    QOL_TEST_ASSERT(str_icmp("apple", "banana") < 0, "apple < banana");
    QOL_TEST_ASSERT(str_icmp("banana", "apple") > 0, "banana > apple");
    QOL_TEST_EQ(str_icmp("", ""), 0, "empty strings equal");
}
