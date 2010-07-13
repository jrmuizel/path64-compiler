/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : messages.h
 *
 * Description : messages dedicated to end-user.
............................................................................
*/

#ifndef utils_messages_h
#define utils_messages_h

/* local includes */
#include "utils_version_id.h"

#ifdef __RELEASE__
#define UTILS_PRINT_ERROR_ASSERT_0 "UT-000", "assertion failed\n"
#define UTILS_PRINT_ERROR_ASSERT_1 "UT-000", "assertion failed: %s\n"
#define UTILS_PRINT_ERROR_ASSERT_2 "UT-000", "assertion failed: %s  - %s\n"
#else
#define UTILS_PRINT_ERROR_ASSERT_0 "UT-000", "at %s:%d assertion failed\n"
#define UTILS_PRINT_ERROR_ASSERT_1 "UT-000", "at %s:%d assertion failed: %s\n"
#define UTILS_PRINT_ERROR_ASSERT_2 "UT-000", "at %s:%d assertion failed: %s - %s\n"
#endif

/* Must be always defined if memory allocator is used */
#define UTILS_PRINT_ERROR_MEMORY_CANNOT_ALLOCATE "UT-100", "cannot allocate memory (%d bytes): %s"

/* Must be always defined if command line parser is used */
#define UTILS_PRINT_ERROR_COMMAND_NOACT          "UT-201", "No action in command line"
#define UTILS_PRINT_ERROR_COMMAND_PARSINGERR     "UT-202", "Parsing error on string %s: %s"
#define UTILS_PRINT_ERROR_COMMAND_MISARG         "UT-210", "Missing argument for option %s"
#define UTILS_PRINT_ERROR_COMMAND_UNKNOWNOPT     "UT-211", "Option %s is unknown or uncorrectly used"
#define UTILS_PRINT_ERROR_COMMAND_TWOARG         "UT-212", "Option %s already specified"
#define UTILS_PRINT_ERROR_COMMAND_NOTCOMPATIBLE  "UT-213", "Options %s and %s are not compatible"

#endif

