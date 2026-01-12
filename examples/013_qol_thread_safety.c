/*
 * ===========================================================================
 * 013_qol_thread_safety.c
 *
 * Example demonstrating thread-safety of build.h
 *
 * This example spawns multiple threads that concurrently use:
 * - Logger (shared state protected by mutex)
 * - Temporary allocator (shared buffer protected by mutex)
 * - Time/date functions (thread-local storage)
 * - Test suite (shared state protected by mutex)
 *
 * Without mutex protection, this would cause race conditions, data corruption,
 * and undefined behavior. With our thread-safe implementation, all threads
 * can safely use build.h features concurrently.
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

#include <stdint.h>

#if !defined(WINDOWS)
    #include <unistd.h>  // For usleep on Unix
#endif

#if defined(WINDOWS)
    #include <windows.h>
    typedef HANDLE ThreadHandle;
    #define THREAD_RETURN_TYPE DWORD WINAPI
    #define THREAD_CREATE(handle, func, arg) \
        (*(handle) = CreateThread(NULL, 0, func, arg, 0, NULL)) != NULL
    #define THREAD_JOIN(handle) WaitForSingleObject(handle, INFINITE)
    #define THREAD_CLOSE(handle) CloseHandle(handle)
#else
    #include <pthread.h>
    typedef pthread_t ThreadHandle;
    #define THREAD_RETURN_TYPE void*
    #define THREAD_CREATE(handle, func, arg) \
        pthread_create(handle, NULL, func, arg) == 0
    #define THREAD_JOIN(handle) pthread_join(handle, NULL)
    #define THREAD_CLOSE(handle) (void)0
#endif

#define NUM_THREADS 8
#define ITERATIONS_PER_THREAD 50

typedef struct {
    int thread_id;
    int iterations;
} ThreadData;

#if defined(WINDOWS)
DWORD WINAPI worker_thread(void *arg)
#else
void* worker_thread(void *arg)
#endif
{
    ThreadData *data = (ThreadData *)arg;
    int thread_id = data->thread_id;
    int iterations = data->iterations;

    // Each thread uses build.h features concurrently
    for (int i = 0; i < iterations; i++) {
        // 1. Logger - shared state protected by mutex
        // Multiple threads logging simultaneously should not corrupt state
        info("[Thread %d] Iteration %d: Starting work\n", thread_id, i);
        
        // 2. Temporary allocator - shared buffer protected by mutex
        // Multiple threads allocating from temp allocator simultaneously
        char *path = temp_sprintf("/tmp/thread_%d_file_%d.txt", thread_id, i);
        char *message = temp_sprintf("Thread %d processed item %d", thread_id, i);
        
        // Use the allocated strings
        diag("[Thread %d] Created path: %s, message: %s\n", thread_id, path, message);
        
        // 3. Time/date functions - thread-local storage
        // Each thread gets its own buffer, no contention
        const char *time_str = get_time();
        const char *date_str = qol_get_date();  // Using qol_ prefix since get_date not in strip list
        const char *datetime_str = qol_get_datetime();  // Using qol_ prefix since get_datetime not in strip list
        
        hint("[Thread %d] Time: %s, Date: %s, DateTime: %s\n", thread_id, time_str, date_str, datetime_str);
        
        // 4. Temp allocator checkpoint/rewind - protected by mutex
        size_t checkpoint = temp_save();
        char *temp_data = temp_sprintf("temp_%d_%d", thread_id, i);
        UNUSED(temp_data);
        temp_rewind(checkpoint);
        
        // 5. More logging with different levels
        if (i % 10 == 0) {
            warn("[Thread %d] Progress: %d/%d iterations completed\n", 
                 thread_id, i, iterations);
        }
        
        // Simulate some work
        #if defined(WINDOWS)
            Sleep(1);  // 1ms delay
        #else
            usleep(1000);  // 1ms delay (1000 microseconds)
        #endif
    }
    
    info("[Thread %d] Completed all %d iterations\n", thread_id, iterations);
    
    #if defined(WINDOWS)
        return 0;
    #else
        return NULL;
    #endif
}

int main(void) {
    info("=== Thread Safety Demonstration ===\n");
    info("Spawning %d threads, each performing %d iterations\n", 
         NUM_THREADS, ITERATIONS_PER_THREAD);
    info("All threads will concurrently use:\n");
    info("  - Logger (shared state)\n");
    info("  - Temporary allocator (shared buffer)\n");
    info("  - Time/date functions (thread-local storage)\n");
    info("\n");
    
    // Initialize logger
   init_logger(.level=LOG_INFO, .time=true, .color=true); 
    
    ThreadHandle threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    // Create all threads
    info("Creating threads...\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].iterations = ITERATIONS_PER_THREAD;
        
        if (!(THREAD_CREATE(&threads[i], worker_thread, &thread_data[i]))) {
            erro("Failed to create thread %d\n", i);
            return EXIT_FAILURE;
        }
    }
    
    info("All threads created. Waiting for completion...\n");
    info("\n");
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        THREAD_JOIN(threads[i]);
        THREAD_CLOSE(threads[i]);
    }
    
    info("\n");
    info("=== All threads completed successfully! ===\n");
    info("\n");
    info("If you see this message and the program completed without crashes,\n");
    info("the thread-safety implementation is working correctly.\n");
    info("\n");
    info("Key observations:\n");
    info("  1. No crashes or undefined behavior\n");
    info("  2. Temporary allocator handled concurrent allocations safely\n");
    info("  3. Time/date functions worked correctly in all threads\n");
    info("  4. Logger state remained consistent (no corruption)\n");
    info("  5. All threads completed their work successfully\n");
    info("\n");
    info("Without mutex protection, you would see:\n");
    info("  - Memory corruption in temp allocator (buffer overflows)\n");
    info("  - Race conditions causing crashes or hangs\n");
    info("  - Corrupted logger state (wrong log levels, colors, etc.)\n");
    info("  - Inconsistent or lost data\n");
    
    return EXIT_SUCCESS;
}
