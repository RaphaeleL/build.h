#include <stdio.h>
#include <stdlib.h>

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

typedef struct {
    int a, b;
    int *result;
    void (*fn)(int, int, int *);
    char sym;
} Calculator;

static void _add(int a, int b, int *c) { *c = a + b; }
static void _sub(int a, int b, int *c) { *c = a - b; }
static void _mul(int a, int b, int *c) { *c = a * b; }
static void _div(int a, int b, int *c) { *c = a / b; }

static void eval(int a, int b, char op) {
    Calculator c;
    switch(op) {
        case '+':
          c = (Calculator) {
            .a = a,
            .b = b,
            .result = malloc(sizeof(int)), .fn = _add,
            .sym = '+'
          };
          break;
        case '-':
          c = (Calculator) {
            .a = a,
            .b = b,
            .result = malloc(sizeof(int)), .fn = _sub,
            .sym = '-'
          };
          break;
        case '/':
          c = (Calculator) {
            .a = a,
            .b = b,
            .result = malloc(sizeof(int)), .fn = _div,
            .sym = '/'
          };
          break;
        case '*':
          c = (Calculator) {
            .a = a,
            .b = b,
            .result = malloc(sizeof(int)), .fn = _mul,
            .sym = '*'
          };
          break;
        default:
          critical("Unknown operator: %c", op);
          return;
    }
    c.fn(c.a, c.b, c.result);
    info("%d %c %d = %d", c.a, c.sym, c.b, *c.result);
    free(c.result);
}

int main(int argc, char **argv) {
    add_argument("--a", "2", "The first number");
    add_argument("--b", "3", "The second number");
    add_argument("--operation", "+", "The operation");

    init_argparser(argc, argv);

    arg_t *a = get_argument("--a");
    arg_t *b = get_argument("--b");
    arg_t *o = get_argument("--operation");

    if (a && b && o) {
        eval(atoi(a->value), atoi(b->value), o->value[0]);
    }

    return EXIT_SUCCESS;
}
