#pragma once

#define _DEFAULT_SOURCE
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _Bool
#define _Bool int
#endif

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef _Bool bool;

typedef void (*FnPtr)(void);

#define true 1
#define false 0
#define ArrayCount(a)     (sizeof(a) / sizeof(a)[0])
#define Min(a, b)         ((a) > (b) ? (b) : (a))
#define Max(a, b)         ((a) > (b) ? (a) : (b))
#define Abs(a)            ((a) < 0 ? -(a) : (a))
#define Align(n, m)       (((u32)(n) + (u32)(m) - 1) & ~((u32)(m) - 1))
#define SetBit(w, b, n)   (((w) & ~(1 << (b))) | ((n) << (b)))
#define SetBits(w, m, n)  (((w) & ~(m)) | ((n) & (m)))
