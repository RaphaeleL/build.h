#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

void print_hm(HashMap* hm) {
    hint("- hm[%s] = %s\n", "name",      (const char*) hm_get(hm, (void*)"name"));
    hint("- hm[%s] = %d\n", "age",       *(int*)       hm_get(hm, (void*)"age"));
    hint("- hm[%s] = %s\n", "city",      (const char*) hm_get(hm, (void*)"city"));
    hint("- hm[%s] = %f\n", "salary",    *(float*)     hm_get(hm, (void*)"salary"));
    hint("- hm[%s] = %b\n", "is_active", *(bool*)      hm_get(hm, (void*)"is_active"));
}

int main(void) {
    // Create a new hashmap
    info("Creating a new hashmap...\n");
    HashMap* hm = hm_create();

    // Add some key-value pairs
    info("Adding entries to the hashmap...\n");

    const char* key1 = "name";
    const char* value1 = "John Doe";
    hm_put(hm, (void*)key1, (void*)value1);

    const char* key2 = "age";
    int age_value = 30;
    hm_put(hm, (void*)key2, (void*)&age_value);

    const char* key3 = "city";
    const char* value3 = "New York";
    hm_put(hm, (void*)key3, (void*)value3);

    const char* key4 = "salary";
    float salary_value = 75000.50f;
    hm_put(hm, (void*)key4, (void*)&salary_value);

    const char* key5 = "is_active";
    bool is_active_value = true;
    hm_put(hm, (void*)key5, (void*)&is_active_value);

    // Get size
    info("HashMap size: %zu\n", hm_size(hm));

    // Print hashmap
    print_hm(hm);

    // Check if key exists
    info("Checking if 'name' exists: %s\n", hm_contains(hm, (void*)key1) ? "Yes" : "No");
    info("Checking if 'city' exists: %s\n", hm_contains(hm, (void*)key3) ? "Yes" : "No");
    info("Checking if 'country' exists: %s\n", hm_contains(hm, (void*)"foo") ? "Yes" : "No");

    // Remove an entry
    info("Removing 'city'...\n");
    hm_remove(hm, (void*)key3);
    info("Checking if 'city' exists: %s\n", hm_contains(hm, (void*)key3) ? "Yes" : "No");
    info("HashMap size after removal: %zu\n", hm_size(hm));

    // Print hashmap
    print_hm(hm);

    // Update an entry
    info("Updating 'name'...\n");
    const char* new_name = "Jane Smith";
    hm_put(hm, (void*)key1, (void*)new_name);

    // Print hashmap
    print_hm(hm);

    // Clear all entries
    info("HashMap size: %zu\n", hm_size(hm));
    info("Clearing all entries...\n");
    hm_clear(hm);
    info("HashMap size: %zu\n", hm_size(hm));
    info("HashMap is empty: %s\n", hm_empty(hm) ? "Yes" : "No");

    // Release the hashmap
    info("Releasing hashmap memory...\n");
    hm_release(hm);

    return 0;
}
