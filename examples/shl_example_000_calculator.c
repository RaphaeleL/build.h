/*
 * calculator.c
 *
 * An overengineered calculator designed to demonstrate the use of pointers
 * within structs.
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
#include "../libaries/001_shl_logger.h"

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

int main() {
  eval(2, 3, '+');
  eval(9, 3, '-');
  eval(5, 2, '*');
  eval(8, 4, '/');

  return 0;
}
