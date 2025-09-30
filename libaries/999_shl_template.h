/*
 * <filename>
 *
 * <Short Explanation what the SHL is doing
 *
 * Created at:  <Date>
 * Author:      <Author>
 *
 *
 * Copyright (c) <year> <date>
 *
 */

#ifndef SHL_TEMPLATE_H // Rename to your actual file
#define SHL_TEMPLATE_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

// Here are your needed includes
// #include <stdio.h>
// #include <stdbool.h>
// #include <string.h>
// #include <stdlib.h>

// Here are your Function definitions, structs and more
// void shl_foo(int hello);

// Strip prefix macros (optional shorter names)
#ifdef SHL_STRIP_PREFIX
  // Here should be the defined functions, structs and more, but stripped
  // #define foo   shl_foo
#endif // SHL_STRIP_PREFIX

// Example how to use the 000_shl_logger.h in a different shl
#define SHL_USE_LOGGER
#include "000_shl_logger.h"

#ifdef SHL_IMPLEMENTATION
    #include <stdio.h>

    // Here is the place to implement the function
    void shl_foo(int hello) {
        info("Hello %d", hello);
    }

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
   }
#endif // __cplusplus

#endif // SHL_TEMPLATE_H
