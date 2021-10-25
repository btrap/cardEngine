// ========================================================================
// File: variable_defines.h
// Date: 2020/02/22
// Revision: 0.0.1
// Creator: Brandon Traphagen
// Notice: (C) Copyright 2019 by Bee Traph, Inc. All Rights Reserved.
// ========================================================================
#pragma once

// #include <stdint.h> // uint8_t, uint16_t, uint32_t, uint64_t, size_t, uintptr_t
// #include <stddef.h>
#ifndef VARIABLE_DEFINES
#define VARIABLE_DEFINES

// guarenteed to be 8bit
typedef unsigned char u8;
typedef unsigned char uint8;
typedef unsigned char byte;
typedef unsigned char uchar;

typedef signed char s8;
typedef signed char sint8;
typedef signed char schar;

// char may be either signed or unsigned
typedef char octet;

// guarenteed to be 16bit
typedef unsigned short u16;
typedef unsigned short uint16;
typedef unsigned short word;
typedef unsigned short ushort;

typedef signed short s16;
typedef signed short sint16;
typedef signed short sshort;

// guarenteed to be <= short
typedef unsigned int uint;
typedef signed int sint;

// guarenteed to be <= int
typedef unsigned long u32;
typedef unsigned long uint32;
typedef unsigned long dword;
typedef unsigned long ulong;

typedef signed long s32;
typedef signed long sint32;
typedef signed long slong;

// guarenteed to be <= long
typedef unsigned long long u64;
typedef unsigned long long uint64;
typedef unsigned long long qword;
typedef unsigned long long ulonglong;

typedef signed long long s64;
typedef signed long long sint64;
typedef signed long long slonglong;

typedef u64 umax;
typedef s64 smax;

// 32 bit percision values
typedef float r32;
typedef float single;
typedef float real32;

// 64 bit percision values
typedef double r64;
typedef double real64;

typedef bool flag;

#endif
