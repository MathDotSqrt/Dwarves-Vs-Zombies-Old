#pragma once
#include <string.h>
#include <stdio.h>

//#define NO_LEVEL	0x00
//#define ERROR_LEVEL	0x01
//#define INFO_LEVEL	0x02
//#define DEBUG_LEVEL	0x03
//
//#define LOG_LEVEL DEBUG_LEVEL
//
//#define ERROR_TAG "ERROR"
//#define INFO_TAG "INFO"
//#define DEBUG_TAG "DEBUG"


#define _FILE strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__

#define NEW_LINE "\n"

#define LOG_FMT             "%-4s | %-15s | %s:%d | "
#define LOG_ARGS(LOG_TAG)   LOG_TAG, _FILE, __FUNCTION__, __LINE__


#define PRINT(format, ...) fprintf (stderr, format, __VA_ARGS__)


//#if LOG_LEVEL >= ERROR_LEVEL
//#define LOG_ERROR(message, ...) PRINT(LOG_FMT message NEW_LINE, LOG_ARGS(ERROR_TAG), __VA_ARGS__)
//#else
//#define LOG_ERROR(message, ...)
//#endif
//
//#if LOG_LEVEL >= INFO_LEVEL
//#define LOG_INFO(message, ...) PRINT(LOG_FMT message NEW_LINE, LOG_ARGS(INFO_TAG), __VA_ARGS__)
//#else
//#define LOG_INFO(message, ...)
//#endif
//
//#if LOG_LEVEL >= DEBUG_LEVEL
//#define LOG_DEBUG(message, ...) PRINT(LOG_FMT message NEW_LINE, LOG_ARGS(DEBUG_TAG), __VA_ARGS__)
//#else
//#define LOG_DEBUG(message, ...)
//#endif

#define LOG_ALWAYS(message, ...)	PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("Chris"), __VA_ARGS__)


#define LOG_ERROR(message, ...)		PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("ERROR"), __VA_ARGS__)
#define LOG_WARNING(message, ...)	PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("WARNING"), __VA_ARGS__)
#define LOG_INFO(message, ...)		PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("INFO"), __VA_ARGS__)
#define LOG_DEBUG(message, ...)		//PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("DEBUG"), __VA_ARGS__)

#define LOG_VOXEL(message, ...)		PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("VOXEL"), __VA_ARGS__)

#define LOG_SYSTEM(message, ...)	PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("SYSTEM"), __VA_ARGS__)
#define LOG_NET(message, ...)		PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("NET"), __VA_ARGS__)
#define LOG_ALLOC(message, ...)		PRINT(LOG_FMT message NEW_LINE, LOG_ARGS("ALLOC"), __VA_ARGS__)

