/*
 * 000_shl_logger.h
 *
 * SHL for a Logger
 *
 * Created at:  29. Sep 2025
 * Author:      Raphaele Salvatore Licciardo
 *
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 *
 */

#ifndef SHL_LOGGER_H
#define SHL_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: log to system log? (like journalctl)

#include <stdarg.h>

// Log levels
typedef enum {
    SHL_LOG_DEBUG = 0,
    SHL_LOG_INFO,
    SHL_LOG_WARN,
    SHL_LOG_ERROR,
    SHL_LOG_CRITICAL,
    SHL_LOG_NONE
} shl_log_level_t;

// Initialize logger with minimum level
void shl_init_logger(shl_log_level_t level);

// Public logging macros
#define shl_debug(fmt, ...) shl_log(SHL_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define shl_info(fmt, ...) shl_log(SHL_LOG_INFO, fmt, ##__VA_ARGS__)
#define shl_warn(fmt, ...) shl_log(SHL_LOG_WARN, fmt, ##__VA_ARGS__)
#define shl_error(fmt, ...) shl_log(SHL_LOG_ERROR, fmt, ##__VA_ARGS__)
#define shl_critical(fmt, ...) shl_log(SHL_LOG_CRITICAL, fmt, ##__VA_ARGS__)

// Strip prefix macros
#ifdef SHL_STRIP_PREFIX
#define init_logger   shl_init_logger
#define debug         shl_debug
#define info          shl_info
#define warn          shl_warn
#define error         shl_error
#define critical      shl_critical
#define LOG_NONE      SHL_LOG_NONE
#define LOG_DEBUG     SHL_LOG_DEBUG
#define LOG_INFO      SHL_LOG_INFO
#define LOG_WARN      SHL_LOG_WARN
#define LOG_ERROR     SHL_LOG_ERROR
#define LOG_CRITICAL  SHL_LOG_CRITICAL
#endif // SHL_STRIP_PREFIX

#ifdef SHL_IMPLEMENTATION
#include <stdio.h>
#include <time.h>

// ANSI colors
#define SHL_COLOR_RESET     "\x1b[0m"
#define SHL_COLOR_DEBUG     "\x1b[90m" // gray
#define SHL_COLOR_INFO      "\x1b[32m" // green
#define SHL_COLOR_WARN      "\x1b[33m" // yellow
#define SHL_COLOR_ERROR     "\x1b[31m" // red
#define SHL_COLOR_CRITICAL  "\x1b[35m" // purple

static shl_log_level_t shl_logger_min_level = SHL_LOG_INFO;

void shl_init_logger(shl_log_level_t level) {
    shl_logger_min_level = level;
}

static const char *shl_level_to_str(shl_log_level_t level) {
    switch (level) {
        case SHL_LOG_DEBUG:    return "DEBUG";
        case SHL_LOG_INFO:     return "INFO";
        case SHL_LOG_WARN:     return "WARN";
        case SHL_LOG_ERROR:    return "ERROR";
        case SHL_LOG_CRITICAL: return "CRITICAL";
        default:               return "UNK";
    }
}

static const char *shl_level_to_color(shl_log_level_t level) {
    switch (level) {
        case SHL_LOG_DEBUG:    return SHL_COLOR_DEBUG;
        case SHL_LOG_INFO:     return SHL_COLOR_INFO;
        case SHL_LOG_WARN:     return SHL_COLOR_WARN;
        case SHL_LOG_ERROR:    return SHL_COLOR_ERROR;
        case SHL_LOG_CRITICAL: return SHL_COLOR_CRITICAL;
        default:               return SHL_COLOR_RESET;
    }
}

void shl_log(shl_log_level_t level, const char *fmt, ...) {
    if (level < shl_logger_min_level || level >= SHL_LOG_NONE) return;

    // Timestamp
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);

    const char *level_str   = shl_level_to_str(level);
    const char *level_color = shl_level_to_color(level);

    // Print colored level, left-aligned in a fixed column
    fprintf(stderr, "%s[%s]%s %s >>> ",
            level_color,
            level_str,
            SHL_COLOR_RESET,
            buf);

    // Print formatted message
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

#endif // SHL_IMPLEMENTATION

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHL_LOGGER_H
