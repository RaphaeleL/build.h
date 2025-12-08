#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_timer_start_and_elapsed) {
    Timer t = {0};
    timer_start(&t);
    
    // Timer should have started
    double elapsed = timer_elapsed(&t);
    QOL_TEST_TRUTHY(elapsed >= 0.0, "timer elapsed should be non-negative");
    
    // Wait a tiny bit
    usleep(1000);  // 1ms
    
    double elapsed_after = timer_elapsed(&t);
    QOL_TEST_TRUTHY(elapsed_after > elapsed, "timer should advance over time");
}

QOL_TEST(test_timer_elapsed_ms) {
    Timer t = {0};
    timer_start(&t);
    
    usleep(5000);  // 5ms
    
    double elapsed_ms = timer_elapsed_ms(&t);
    QOL_TEST_TRUTHY(elapsed_ms >= 4.0 && elapsed_ms < 100.0, "elapsed_ms should be around 5ms");
}

QOL_TEST(test_timer_elapsed_us) {
    Timer t = {0};
    timer_start(&t);
    
    usleep(1000);  // 1ms = 1000us
    
    double elapsed_us = timer_elapsed_us(&t);
    QOL_TEST_TRUTHY(elapsed_us >= 500.0 && elapsed_us < 10000.0, "elapsed_us should be around 1000us");
}

QOL_TEST(test_timer_elapsed_ns) {
    Timer t = {0};
    timer_start(&t);
    
    usleep(1000);  // 1ms = 1,000,000ns
    
    uint64_t elapsed_ns = timer_elapsed_ns(&t);
    QOL_TEST_TRUTHY(elapsed_ns >= 500000ULL && elapsed_ns < 10000000ULL, "elapsed_ns should be around 1,000,000ns");
}

QOL_TEST(test_timer_reset) {
    Timer t = {0};
    timer_start(&t);
    
    usleep(10000);  // 10ms
    
    double elapsed_before = timer_elapsed_ms(&t);
    QOL_TEST_TRUTHY(elapsed_before >= 8.0, "should have elapsed at least 8ms");
    
    timer_reset(&t);
    
    double elapsed_after = timer_elapsed_ms(&t);
    QOL_TEST_TRUTHY(elapsed_after < elapsed_before, "reset should restart timer");
    QOL_TEST_TRUTHY(elapsed_after < 5.0, "reset timer should be very small");
}

QOL_TEST(test_timer_null_safety) {
    Timer *null_timer = NULL;
    
    // Should not crash
    timer_start(null_timer);
    double elapsed = timer_elapsed(null_timer);
    QOL_TEST_EQ(elapsed, 0.0, "null timer should return 0");
    
    double elapsed_ms = timer_elapsed_ms(null_timer);
    QOL_TEST_EQ(elapsed_ms, 0.0, "null timer ms should return 0");
    
    double elapsed_us = timer_elapsed_us(null_timer);
    QOL_TEST_EQ(elapsed_us, 0.0, "null timer us should return 0");
    
    uint64_t elapsed_ns = timer_elapsed_ns(null_timer);
    QOL_TEST_EQ(elapsed_ns, 0ULL, "null timer ns should return 0");
    
    timer_reset(null_timer);
    QOL_TEST_TRUTHY(true, "null timer reset should not crash");
}

QOL_TEST(test_timer_precision) {
    Timer t = {0};
    timer_start(&t);
    
    // Very short sleep
    usleep(100);  // 100 microseconds
    
    uint64_t ns1 = timer_elapsed_ns(&t);
    uint64_t ns2 = timer_elapsed_ns(&t);
    
    // Second call should be >= first (or very close due to timing)
    QOL_TEST_TRUTHY(ns2 >= ns1 || (ns1 - ns2 < 1000), "timer should be monotonic or very close");
}
