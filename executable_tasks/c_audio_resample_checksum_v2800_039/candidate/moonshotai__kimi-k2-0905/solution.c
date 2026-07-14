#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* 32-bit fixed-point interpolation: 15 bits fractional part */
#define FRAC_BITS 15
#define FRAC_UNIT (1<<FRAC_BITS)
#define FRAC_MASK (FRAC_UNIT-1)

static uint64_t checksum_i16(const int16_t* data, size_t n)
{
    uint64_t h = 1469598103934665603ULL;
    size_t i = 0;
    /* process 4 samples per iteration */
    for (; i + 3 < n; i += 4) {
        uint64_t v = *(uint64_t*)(data + i);
        h ^= v;
        h *= 1099511628211ULL;
    }
    /* tail */
    for (; i < n; ++i) {
        h ^= (uint16_t)data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters)
{
    static __thread int16_t* out = NULL;
    static __thread size_t out_cap = 0;

    if ((size_t)output_len > out_cap) {
        out_cap = output_len;
        out = (int16_t*)realloc(out, out_cap * sizeof(int16_t));
        if (!out) return 0;
    }

    const int in_last  = input_len - 1;
    const int out_last = output_len - 1;
    const uint32_t ratio = ((uint64_t)in_last << FRAC_BITS) / out_last;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int16_t* dst = out;
        uint32_t pos = 0;
        for (int i = 0; i < output_len; ++i) {
            int left  = (int)(pos >> FRAC_BITS);
            int right = left + 1;
            if (right >= input_len) right = in_last;
            uint32_t frac = pos & FRAC_MASK;
            int32_t l = src[left];
            int32_t r = src[right];
            int32_t v = l + (((r - l) * (int32_t)frac) >> FRAC_BITS);
            dst[i] = (int16_t)v;
            pos += ratio;
        }
        hash = checksum_i16(dst, (size_t)output_len);
    }
    return hash;
}