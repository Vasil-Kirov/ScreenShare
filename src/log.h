#pragma once

#include "vstring.h"
#include <stdio.h>
#include <assert.h>

typedef enum {
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
} LogLevel;

StringBuilder log_prefix(LogLevel level, const char *fmt);
void _log(LogLevel level, const char *fmt, ...);
void set_log_handle(FILE *f);


#define lfatal(fmt, ...) { _log(LOG_ERROR, fmt, __VA_ARGS__); assert(false); }
#define lerror(fmt, ...) _log(LOG_ERROR, fmt, __VA_ARGS__)
#define lwarn(fmt, ...)  _log(LOG_WARN, fmt, __VA_ARGS__)
#define linfo(fmt, ...)  _log(LOG_INFO, fmt, __VA_ARGS__)
#define ldebug(fmt, ...) _log(LOG_DEBUG, fmt, __VA_ARGS__)

