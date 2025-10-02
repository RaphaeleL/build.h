/*
 * ===========================================================================
 * 004_shl_helper.h
 *
 * SHL: A useful set of helper functionalities for C/C++ projects.
 *
 * Provides:
 *  - Assertion wrapper macros (SHL_ASSERT)
 *  - Utilities for marking unused variables (SHL_UNUSED)
 *  - TODO and unreachable markers (SHL_TODO, SHL_UNREACHABLE)
 *  - Array helpers (SHL_ARRAY_LEN, SHL_ARRAY_GET)
 *  - Optional shorter macros if SHL_STRIP_PREFIX is defined
 *
 * Dependencies:
 *  - <assert.h> (optional, if SHL_ASSERT not defined)
 *  - <stdio.h>, <stdlib.h>
 *
 * Configuration:
 *  - Define SHL_STRIP_PREFIX to use shorter macro names
 *
 * Usage:
 *  - Include this header wherever helper macros are needed.
 *  - Define SHL_IMPLEMENTATION in *one* source file to add function implementations if needed.
 *
 * Notes:
 *  - Lightweight, header-only utilities.
 *  - Designed to be portable between C and C++.
 *
 * Limitations:
 *  - Minimal runtime checks; only basic assertion and array bounds checking.
 *  - Intended for small-scale helpers, not full libraries.
 *
 * History:
 *  - n/a
 *
 * Created: 02 Oct 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#ifndef SHL_HELPER_H
#define SHL_HELPER_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>

#ifndef SHL_ASSERT
    #include <assert.h>
    #define SHL_ASSERT assert
#endif /* NOB_ASSERT */

#define SHL_UNUSED(value) (void)(value)
#define SHL_TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define SHL_UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define SHL_ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))
#define SHL_ARRAY_GET(array, index) \
    (SHL_ASSERT((size_t)(index) < SHL_ARRAY_LEN(array)), (array)[(size_t)(index)])


// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
    #define ASSERT              SHL_ASSERT
    #define UNUSED              SHL_UNUSED
    #define TODO                SHL_TODO
    #define UNREACHABLE         SHL_UNREACHABLE
    #define ARRAY_LEN           SHL_ARRAY_LEN
    #define ARRAY_GET           SHL_ARRAY_GET
#endif // SHL_STRIP_PREFIX


#ifdef SHL_IMPLEMENTATION

// TODO

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // SHL_HELPER_H
