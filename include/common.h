#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum
{
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_BUSY = -2,
    STATUS_TIMEOUT = -3,
    STATUS_INVALID_PARAM = -4,
    STATUS_NOT_INITIALIZED = -5,
    STATUS_BUFFER_OVERFLOW = -6,
    STATUS_NOT_FOUND = -7
} Status_t;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef uint32_t Tick_t;

static inline Tick_t Tick_Diff(Tick_t now, Tick_t before)
{
    return (now >= before) ? (now - before) : (UINT32_MAX - before + 1 + now);
}

static inline bool Tick_Elapsed(Tick_t now, Tick_t start, Tick_t duration_ms)
{
    return Tick_Diff(now, start) >= duration_ms;
}

#endif