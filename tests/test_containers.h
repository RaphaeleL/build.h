#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

QOL_TEST(test_stack_push_pop) {
    qol_stack(int) s = {0};
    stack_push(&s, 1);
    stack_push(&s, 2);
    stack_push(&s, 3);
    QOL_TEST_EQ(s.len, 3, "stack length");
    QOL_TEST_EQ(stack_top(&s), 3, "stack top");
    int val = 0;
    stack_pop(&s, &val);
    QOL_TEST_EQ(val, 3, "stack pop value");
    QOL_TEST_EQ(s.len, 2, "stack length after pop");
    stack_release(&s);
}

QOL_TEST(test_queue_push_pop) {
    qol_queue(int) q = {0};
    queue_push(&q, 10);
    queue_push(&q, 20);
    queue_push(&q, 30);
    QOL_TEST_EQ(queue_len(&q), 3, "queue length");
    int val = 0;
    queue_pop(&q, &val);
    QOL_TEST_EQ(val, 10, "queue pop fifo order");
    queue_pop(&q, &val);
    QOL_TEST_EQ(val, 20, "queue pop second");
    QOL_TEST_EQ(queue_len(&q), 1, "queue length after pops");
    queue_release(&q);
}

QOL_TEST(test_ring_push_pop) {
    qol_ring(int) r = {0};
    ring_init(&r, 3);
    ring_push(&r, 1);
    ring_push(&r, 2);
    ring_push(&r, 3);
    QOL_TEST_TRUTHY(ring_full(&r), "ring is full");
    int val = 0;
    ring_pop(&r, &val);
    QOL_TEST_EQ(val, 1, "ring pop fifo order");
    ring_push(&r, 4);
    ring_pop(&r, &val);
    QOL_TEST_EQ(val, 2, "ring wrap-around");
    ring_free(&r);
}
