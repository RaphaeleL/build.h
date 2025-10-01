/*
 * ===========================================================================
 * 003_shl_dynamic_array.h
 *
 * SHL: Dynamic Arrays in C.
 *
 * Provides:
 *  - grow(vec, n)        : Reserve memory for at least n elements
 *  - shrink(vec)         : Shrink allocated memory if much is unused
 *  - push(vec, val)      : Append a single element
 *  - pushn(vec, src, n)  : Append multiple elements from an array
 *  - drop(vec)           : Remove the last element
 *  - dropn(vec, n)       : Remove element at index n, shifting elements down
 *  - remove_swap(vec, i) : Remove element at index i by swapping with last
 *  - resize(vec, n)      : Resize array to exactly n elements (may grow)
 *  - release(vec)        : Free all allocated memory
 *  - back(vec)           : Get last element (asserts non-empty)
 *  - list(T)             : Macro to declare a dynamic array of type T
 *
 * Dependencies:
 *  - Standard C library headers: stdio.h, stdlib.h, string.h, stdbool.h, ctype.h
 *  - Optional logger: 000_shl_logger.h (enabled with SHL_USE_LOGGER)
 *
 * Configuration:
 *  - SHL_STRIP_PREFIX: If defined, strips 'SHL_' prefix for shorter names
 *  - SHL_IMPLEMENTATION: Define in one source file to include implementation
 *  - SHL_USE_LOGGER: Enable logging macros (info, debug, error)
 *
 * Usage:
 *  #define SHL_IMPLEMENTATION
 *  #include "003_shl_dynamic_array.h"
 *
 * Notes:
 *  - Only one source file should define SHL_IMPLEMENTATION
 *  - Autorun will execute the output binary automatically after a successful build
 *  - Logging requires SHL_USE_LOGGER and 000_shl_logger.h present
 *  - Automatic memory growth and optional shrinking are handled internally
 *  - drop() and dropn() will automatically shrink memory if the array is under-utilized
 *
 * Limitations:
 *  - Does not automatically shrink below SHL_INIT_CAP
 *  - Not thread-safe; concurrent access requires external synchronization
 *  - Array elements are stored in contiguous memory, so pushn() may trigger reallocation
 *
 * History:
 *  - 01 Oct 2025: Created by Raphaele Salvatore Licciardo
 *
 * Created: 01 Oct 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#ifndef SHL_DYNAMIC_ARRAY_H
#define SHL_DYNAMIC_ARRAY_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SHL_USE_LOGGER
#include "000_shl_logger.h"

#define SHL_INIT_CAP 8

// Reserve space for at least `n` elements
#define shl_grow(vec, n)                                                                    \
    do {                                                                                    \
        if ((n) > (vec)->cap) {                                                             \
            size_t newcap = (vec)->cap ? (vec)->cap : SHL_INIT_CAP;                         \
            while (newcap < (n)) newcap *= 2;                                               \
            if ((vec)->cap == 0) {                                                          \
                debug("Dynamic array inits memory on %d.\n", newcap);                       \
            } else {                                                                        \
                debug("Dynamic array needs more memory (%d -> %d)!\n", (vec)->cap, newcap); \
            }                                                                               \
            void *tmp = realloc((vec)->data, newcap * sizeof(*(vec)->data));                \
            if (!tmp) {                                                                     \
                error("Dynamic array out of memory (need %zu elements)\n", n);              \
                abort();                                                                    \
            }                                                                               \
            (vec)->data = tmp;                                                              \
            (vec)->cap = newcap;                                                            \
        }                                                                                   \
    } while (0)

#define shl_shrink(vec)                                                                           \
    do {                                                                                          \
        if ((vec)->len < (vec)->cap / 2 && (vec)->cap > SHL_INIT_CAP) {                           \
            size_t newcap = (vec)->cap / 2;                                                       \
            debug("Dynamic array can release some memory (%d -> %d)!\n", (vec)->cap, newcap); \
            void *tmp = realloc((vec)->data, newcap * sizeof(*(vec)->data));                      \
            if (tmp) {                                                                            \
                (vec)->data = tmp;                                                                \
                (vec)->cap = newcap;                                                              \
            }                                                                                     \
        }                                                                                         \
    } while (0)

// Push a single item
#define shl_push(vec, val)                 \
    do {                                   \
        shl_grow((vec), (vec)->len+1);     \
        (vec)->data[(vec)->len++] = (val); \
    } while (0)

// Push multiple items at once
#define shl_pushn(vec, src, count)                                             \
    do {                                                                       \
        size_t __cnt = (count);                                                \
        shl_grow((vec), (vec)->len + __cnt);                                   \
        memcpy((vec)->data + (vec)->len, (src), __cnt * sizeof(*(vec)->data)); \
        (vec)->len += __cnt;                                                   \
    } while (0)

// Remove the last element
#define shl_drop(vec)                             \
    do {                                          \
        if ((vec)->len == 0) {                    \
            error("shl_drop() on empty array\n"); \
            abort();                              \
        }                                         \
        --(vec)->len;                             \
        shrink(vec);                              \
    } while (0)

// Remove element at index n (shift elements down)
#define shl_dropn(vec, n)                                         \
    do {                                                          \
        size_t __idx = (n);                                       \
        if (__idx >= (vec)->len) {                                \
            error("shl_dropn(): index out of range\n");           \
            abort();                                              \
        }                                                         \
        memmove((vec)->data + __idx,                              \
                (vec)->data + __idx + 1,                          \
                ((vec)->len - __idx - 1) * sizeof(*(vec)->data)); \
        --(vec)->len;                                             \
        shrink(vec);                                              \
    } while (0)

// Resize array to exactly n elements (uninitialized if grown)
#define shl_resize(vec, n)    \
    do {                      \
        shl_grow((vec), (n)); \
        (vec)->len = (n);     \
    } while (0)

// Free the buffer
#define shl_release(vec)             \
    do {                             \
        free((vec)->data);           \
        (vec)->data = NULL;          \
        (vec)->len = (vec)->cap = 0; \
    } while (0)

// Get last element (asserts non-empty)
#define shl_back(vec) \
    ((vec)->len > 0 ? (vec)->data[(vec)->len-1] : (error("shl_back() on empty array\n"), abort(), (vec)->data[0]))

// Remove element at index i (swap with last)
#define shl_remove_swap(vec, i)                         \
    do {                                                \
        size_t __idx = (i);                             \
        if (__idx >= (vec)->len) {                      \
            error("shl_remove_swap(): out of range\n"); \
            abort();                                    \
        }                                               \
        (vec)->data[__idx] = (vec)->data[--(vec)->len]; \
    } while (0)

// Struct wrapper
#define shl_list(T) \
    struct { T *data; size_t len, cap; }

// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
    #define grow         shl_grow
    #define shrink       shl_shrink
    #define push         shl_push
    #define pushn        shl_pushn
    #define drop         shl_drop
    #define dropn        shl_dropn
    #define resize       shl_resize
    #define release      shl_release
    #define back         shl_back
    #define remove_swap  shl_remove_swap
    #define list         shl_list
#endif // SHL_STRIP_PREFIX

#ifdef SHL_IMPLEMENTATION

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // SHL_DYNAMIC_ARRAY_H
