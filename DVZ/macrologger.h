#pragma once
#include <string.h>

#define NO_LEVEL	0x00
#define ERROR_LEVEL	0x01
#define INFO_LEVEL	0x02
#define DEBUG_LEVEL	0x03

#define LOG_LEVEL DEBUG_LEVEL

#define ERROR_TAG "ERROR"
#define INFO_TAG "INFO"
#define DEBUG_TAG "DEBUG"


#define _FILE strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__

#define NEW_LINE "\n"

#define LOG_FMT             "%s | %-7s | %-15s | %s:%d | "
#define LOG_ARGS(LOG_TAG)   __TIME__, LOG_TAG, _FILE, __FUNCTION__, __LINE__


#define PRINT(format, ...) fprintf (stderr, format, __VA_ARGS__)


#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(message, ...) PRINT(LOG_FMT message NEW_LINE, LOG_ARGS(ERROR_TAG), __VA_ARGS__)
#else
#define LOG_ERROR(message, args...)
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(message, ...) PRINT(LOG_FMT message NEW_LINE, LOG_ARGS(INFO_TAG), __VA_ARGS__)
#else
#define LOG_INFO(message, args...)
#endif

#if LOG_LEVEL >= DEBUG_LEVEL
#define LOG_DEBUG(message, ...) PRINT(LOG_FMT message NEW_LINE, LOG_ARGS(DEBUG_TAG), __VA_ARGS__)
#else
#define LOG_DEBUG(message, args...)
#endif