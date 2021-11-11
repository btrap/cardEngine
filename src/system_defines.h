// ========================================================================
// File: system_defines.h
// Date: 2020/02/24
// Revision: 0.0.1
// Creator: Brandon Traphagen
// Notice: (C) Copyright 2019 by Bee Traph, Inc. All Rights Reserved.
// ========================================================================
#pragma once

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif

#if !defined(COMPILER_UNKNOWN)
#define COMPILER_UNKNOWN 0
#endif

// Determine Compiler verison
#if !COMPILER_MSVC && !COMPILER_LLVM && !COMPILER_GCC
    #if _MSC_VER
        #undef COMPILER_MSVC
        #define COMPILER_MSVC 1
    #elif __clang__
        #undef COMPILER_LLVM
        #define COMPILER_LLVM 1
    #elif __GNUC__
        #undef COMPILER_GCC
        #define COMPILER_GCC 1
    #else
        #undef COMPILER_UNKNOWN
        #define COMPILER_UNKNOWN 1
    #endif
#endif

#define FUNC_SIG inline

#if defined(_DEBUG) || defined(DEBUG)
#define DEBUG_ACTIVE
#endif

#ifdef DEBUG_ACTIVE
#define ASSERT(expression) if(!(expression)) {*(int *)0 = 0;}
#else
#define ASSERT(expression) ((void)0)
#endif
#define ASSERT_NOW() ASSERT(false)

#if COMPILER_UNKNWON
ASSERT_NOW()
#endif

#define MINIMUM(A, B) ((A < B) ? (A) : (B))
#define MAXIMUM(A, B) ((A > B) ? (A) : (B))

#define KILOBYTES(value) ((value)*1024ULL)
#define MEGABYTES(value) (KILOBYTES(value)*1024ULL)
#define GIGABYTES(value) (MEGABYTES(value)*1024ULL)
#define TERABYTES(value) (GIGABYTES(value)*1024ULL)

#define PROPERTY_OFFSET(type, Member) (umax)&(((type *)0)->Member)
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

#define FILE_AND_LINE__(A, B) A "|" #B
#define FILE_AND_LINE_(A, B) FILE_AND_LINE__(A, B)
#define FILE_AND_LINE FILE_AND_LINE_(__FILE__, __LINE__)

#include <stdio.h>
#include <stdarg.h>
static void Print_Console(char* format_Message, ...)
{
    va_list variables;
    va_start(variables, format_Message);
    char out_Message[255];
    vsprintf(out_Message, format_Message, variables);
    va_end(variables);
    
//#if COMPILER_MSVC
//    OutputDebugStringA(out_Message);
//#else
    printf(out_Message);
//#endif
}

#ifdef DEBUG_ACTIVE
    #if COMPILER_MSVC 
    #define PRINT_CONSOLE(message, ...) Print_Console((char*)message, __VA_ARGS__)
    #else
    #define PRINT_CONSOLE(message, ...) Print_Console((char*)message, ##__VA_ARGS__)
    #endif
#else
#define PRINT_CONSOLE(message, ...) ((void)0)
#endif

