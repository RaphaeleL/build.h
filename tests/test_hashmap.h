#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_hashmap_create) {
    HashMap* hm = hm_create();
    QOL_TEST_TRUTHY(hm != NULL, "hashmap created");
    QOL_TEST_EQ(hm_size(hm), 0, "initial size is 0");
    QOL_TEST_TRUTHY(hm_empty(hm), "hashmap is initially empty");
    hm_release(hm);
}

QOL_TEST(test_hashmap_put_and_get) {
    HashMap* hm = hm_create();
    
    const char* key1 = "name";
    const char* value1 = "John Doe";
    hm_put(hm, (void*)key1, (void*)value1);
    
    QOL_TEST_EQ(hm_size(hm), 1, "size after adding one entry");
    QOL_TEST_STREQ((const char*)hm_get(hm, (void*)key1), value1, "get returns correct value");
    
    const char* key2 = "age";
    int age_value = 30;
    hm_put(hm, (void*)key2, (void*)&age_value);
    
    QOL_TEST_EQ(hm_size(hm), 2, "size after adding second entry");
    QOL_TEST_EQ(*(int*)hm_get(hm, (void*)key2), age_value, "get returns correct integer value");
    
    hm_release(hm);
}

QOL_TEST(test_hashmap_contains) {
    HashMap* hm = hm_create();
    
    const char* key1 = "name";
    const char* value1 = "John Doe";
    hm_put(hm, (void*)key1, (void*)value1);
    
    QOL_TEST_TRUTHY(hm_contains(hm, (void*)key1), "contains returns true for existing key");
    QOL_TEST_FALSY(hm_contains(hm, (void*)"nonexistent"), "contains returns false for non-existing key");
    
    hm_release(hm);
}

QOL_TEST(test_hashmap_remove) {
    HashMap* hm = hm_create();
    
    const char* key1 = "name";
    const char* value1 = "John Doe";
    hm_put(hm, (void*)key1, (void*)value1);
    
    const char* key2 = "city";
    const char* value2 = "New York";
    hm_put(hm, (void*)key2, (void*)value2);
    
    QOL_TEST_EQ(hm_size(hm), 2, "size before removal");
    QOL_TEST_TRUTHY(hm_remove(hm, (void*)key1), "remove returns true for existing key");
    QOL_TEST_EQ(hm_size(hm), 1, "size decreases after removal");
    QOL_TEST_FALSY(hm_contains(hm, (void*)key1), "removed key no longer exists");
    QOL_TEST_TRUTHY(hm_contains(hm, (void*)key2), "other key still exists");
    
    QOL_TEST_FALSY(hm_remove(hm, (void*)"nonexistent"), "remove returns false for non-existing key");
    
    hm_release(hm);
}

QOL_TEST(test_hashmap_update) {
    HashMap* hm = hm_create();
    
    const char* key = "name";
    const char* old_value = "John Doe";
    const char* new_value = "Jane Smith";
    
    hm_put(hm, (void*)key, (void*)old_value);
    QOL_TEST_EQ(hm_size(hm), 1, "size before update");
    QOL_TEST_STREQ((const char*)hm_get(hm, (void*)key), old_value, "initial value correct");
    
    hm_put(hm, (void*)key, (void*)new_value);
    QOL_TEST_EQ(hm_size(hm), 1, "size remains same after update");
    QOL_TEST_STREQ((const char*)hm_get(hm, (void*)key), new_value, "value updated correctly");
    
    hm_release(hm);
}

QOL_TEST(test_hashmap_clear) {
    HashMap* hm = hm_create();
    
    hm_put(hm, (void*)"key1", (void*)"value1");
    hm_put(hm, (void*)"key2", (void*)"value2");
    hm_put(hm, (void*)"key3", (void*)"value3");
    
    QOL_TEST_EQ(hm_size(hm), 3, "size before clear");
    
    hm_clear(hm);
    
    QOL_TEST_EQ(hm_size(hm), 0, "size is 0 after clear");
    QOL_TEST_TRUTHY(hm_empty(hm), "hashmap is empty after clear");
    QOL_TEST_FALSY(hm_contains(hm, (void*)"key1"), "keys no longer exist after clear");
    
    hm_release(hm);
}

QOL_TEST(test_hashmap_multiple_types) {
    HashMap* hm = hm_create();
    
    // String
    const char* str_key = "name";
    const char* str_value = "Alice";
    hm_put(hm, (void*)str_key, (void*)str_value);
    QOL_TEST_STREQ((const char*)hm_get(hm, (void*)str_key), str_value, "string value stored correctly");
    
    // Integer
    const char* int_key = "age";
    int int_value = 25;
    hm_put(hm, (void*)int_key, (void*)&int_value);
    QOL_TEST_EQ(*(int*)hm_get(hm, (void*)int_key), int_value, "integer value stored correctly");
    
    // Float
    const char* float_key = "salary";
    float float_value = 75000.50f;
    hm_put(hm, (void*)float_key, (void*)&float_value);
    QOL_TEST_EQ(*(float*)hm_get(hm, (void*)float_key), float_value, "float value stored correctly");
    
    // Boolean
    const char* bool_key = "is_active";
    bool bool_value = true;
    hm_put(hm, (void*)bool_key, (void*)&bool_value);
    QOL_TEST_EQ(*(bool*)hm_get(hm, (void*)bool_key), bool_value, "boolean value stored correctly");
    
    QOL_TEST_EQ(hm_size(hm), 4, "all types stored correctly");
    
    hm_release(hm);
}

QOL_TEST(test_hashmap_empty_operations) {
    HashMap* hm = hm_create();
    
    QOL_TEST_FALSY(hm_contains(hm, (void*)"anykey"), "contains on empty hashmap returns false");
    QOL_TEST_EQ(hm_get(hm, (void*)"anykey"), NULL, "get on empty hashmap returns NULL");
    QOL_TEST_FALSY(hm_remove(hm, (void*)"anykey"), "remove on empty hashmap returns false");
    
    // Clear should work on empty hashmap
    hm_clear(hm);
    QOL_TEST_EQ(hm_size(hm), 0, "clear on empty hashmap maintains zero size");
    
    hm_release(hm);
}
