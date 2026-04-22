/*
 * ===========================================================================
 * 015_qol_cli.c
 *
 * Example usage for qol cli commands 
 *
 * Created: 22 Apr 2026
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2026 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#include <stdio.h>

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"


int main() {
    Cmd cmd = {0};
    
    push(&cmd, "echo", "Hello World");
    run(&cmd);

    return EXIT_SUCCESS;
}

