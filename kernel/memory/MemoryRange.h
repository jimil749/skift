#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/memory/Paging.h"

typedef struct
{
    uintptr_t base;
    size_t size;
} MemoryRange;

inline bool memory_range_empty(MemoryRange range)
{
    return range.size == 0;
}

static inline MemoryRange memory_range_from_non_aligned_address(uintptr_t base, size_t size)
{
    size_t align = PAGE_SIZE - base % PAGE_SIZE;

    if (base % PAGE_SIZE == 0)
    {
        align = 0;
    }

    base += align;
    size -= align;

    size -= size % PAGE_SIZE;

    return (MemoryRange){base, size};
}

static inline MemoryRange memory_range_around_non_aligned_address(uintptr_t base, size_t size)
{
    size_t align = base % PAGE_SIZE;

    base -= align;
    size += align;

    size += PAGE_SIZE - size % PAGE_SIZE;

    return (MemoryRange){base, size};
}
