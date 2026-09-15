/*
 * ===========================================================================
 * 016_qol_containers.c
 *
 * Example usage for stack, queue, and ring buffer containers
 *
 * Created: 2026
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2026 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

int main() {
    info("Example 1: Stack (LIFO)\n");
    qol_stack(int) s = {0};
    stack_push(&s, 10);
    stack_push(&s, 20);
    stack_push(&s, 30);
    int val = 0;
    stack_pop(&s, &val);
    info("Popped from stack: %d\n", val);
    stack_release(&s);

    info("Example 2: Queue (FIFO)\n");
    qol_queue(const char *) q = {0};
    queue_push(&q, "first");
    queue_push(&q, "second");
    queue_push(&q, "third");
    const char *item = NULL;
    queue_pop(&q, &item);
    info("Dequeued: %s\n", item);
    queue_release(&q);

    info("Example 3: Ring buffer (fixed capacity)\n");
    qol_ring(int) r = {0};
    ring_init(&r, 3);
    ring_push(&r, 1);
    ring_push(&r, 2);
    ring_push(&r, 3);
    ring_pop(&r, &val);
    info("Ring pop: %d, then push 4\n", val);
    ring_push(&r, 4);
    ring_pop(&r, &val);
    info("Ring pop after wrap: %d\n", val);
    ring_free(&r);

    return EXIT_SUCCESS;
}
