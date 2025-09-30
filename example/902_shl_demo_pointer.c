/*
 * pointer.c
 *
 * Demonstrattion of pointers.
 *
 * Created at:  29. Sep 2025
 * Author:      Raphaele Salvatore Licciardo
 *
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../libaries/000_shl_logger.h"

typedef struct {
    int a, b;
    void (*fn)(int, int, int*);
} Calculator_v1;

typedef struct {
    int a, b;
    int (*fn)(int, int);
} Calculator_v2;

int apply(int (*fn)(int, int), int x, int y) {
    return fn(x, y);
}

void add_v1(int a, int b, int *result) {
    *result = a + b;
}

static int add_v2(int a, int b) {
    return a + b;
}

void example_a() {
    int result = 0;
    int a = 2;
    int b = 3;
    add_v1(a, b, &result);
    info("a) %d + %d = %d", a, b, result);
}

void example_b() {
    int tmp;
    int* result = &tmp;
    int a = 2;
    int b = 3;
    add_v1(a, b, result);
    info("b) %d + %d = %d", a, b, *result);
}

void example_c() {
    int* result = malloc(sizeof(int));
    int a = 2;
    int b = 3;
    add_v1(a, b, result);
    info("c) %d + %d = %d", a, b, *result);
    free(result);
}

void example_d() {
    int results[3];
    int* ptr = results;

    add_v1(2, 3, ptr);
    add_v1(4, 5, ptr + 1);
    add_v1(10, 20, ptr + 2);

    info("d) %d, %d, %d", results[0], results[1], results[2]);
}

void example_e() {
    int n = 3;
    int* results = malloc(n * sizeof(int));

    add_v1(2, 3, &results[0]);
    add_v1(4, 5, &results[1]);
    add_v1(10, 20, &results[2]);

    info("e) %d, %d, %d", results[0], results[1], results[2]);

    free(results);
}

void example_f() {
    Calculator_v1 *calculator = malloc(sizeof(Calculator_v1));
    calculator->a = 2;
    calculator->b = 3;
    calculator->fn = add_v1;

    int result = 0;
    calculator->fn(calculator->a, calculator->b, &result);
    info("f) %d + %d = %d", calculator->a, calculator->b, result);

    free(calculator);
}

void example_g() {
    Calculator_v2 *calculator = malloc(sizeof(Calculator_v2));
    calculator->a = 2;
    calculator->b = 3;
    calculator->fn = add_v2;

    int result = calculator->fn(calculator->a, calculator->b);
    info("g) %d + %d = %d", calculator->a, calculator->b, result);

    free(calculator);
}

int main() {
    example_a(); // stack
    example_b(); // stack allocation
    example_c(); // heap allocation
    example_d(); // stack allocated array of 3 ints
    example_e(); // heap allocated array of 3 ints
    example_f(); // struct with function pointer
    example_g(); // struct with function pointer, hidden function itself

    return 0;
}
