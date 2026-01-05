/*
 * ===========================================================================
 * 000_qol_logger.c
 *
 * Example usage for QOL_AUTO_FREE
 *
 * Created: 05 Jan 2026
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#include <stdio.h>

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

int main() {
    init_logger(LOG_DIAG, false, true);

    AUTO_FREE int *data = malloc(sizeof(int));

    *data = 42;
    info("data = %d\n", *data);
    *data = 69;
    info("data = %d\n", *data);

    return EXIT_SUCCESS;
}
