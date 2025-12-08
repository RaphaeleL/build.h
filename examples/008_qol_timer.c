/*
 * ===========================================================================
 * 008_qol_timer.c
 *
 * Example usage for QOL High-Resolution Timer
 *
 * Created: 31 Oct 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#include <stdio.h>
#include <unistd.h>

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

int main() {
    Timer t = {0};
    
    // Start timer
    timer_start(&t);
    info("Timer started\n");
    
    // Simulate some work
    info("Sleeping for 1 second...\n");
    sleep(1);
    
    // Get elapsed time in different units
    double elapsed_sec = timer_elapsed(&t);
    double elapsed_ms = timer_elapsed_ms(&t);
    double elapsed_us = timer_elapsed_us(&t);
    uint64_t elapsed_ns = timer_elapsed_ns(&t);
    
    info("Elapsed time:\n");
    info("  %.6f seconds\n", elapsed_sec);
    info("  %.3f milliseconds\n", elapsed_ms);
    info("  %.0f microseconds\n", elapsed_us);
    info("  %llu nanoseconds\n", (unsigned long long)elapsed_ns);
    
    // Reset timer
    info("Resetting timer...\n");
    timer_reset(&t);
    
    // Do more work
    info("Sleeping for 500 milliseconds...\n");
    usleep(500000);  // 500ms
    
    elapsed_ms = timer_elapsed_ms(&t);
    info("Elapsed after reset: %.3f milliseconds\n", elapsed_ms);
    
    // Benchmark example: measure a loop
    info("Benchmarking a loop:\n");
    timer_reset(&t);
    
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    
    elapsed_us = timer_elapsed_us(&t);
    info("Loop of 1,000,000 iterations took %.2f microseconds\n", elapsed_us);
    info("(Sum: %d, to prevent optimization)\n", sum);
    
    return EXIT_SUCCESS;
}
