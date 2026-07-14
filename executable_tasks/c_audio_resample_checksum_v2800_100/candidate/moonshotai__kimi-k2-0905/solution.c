#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    /* on-stack buffer, avoids malloc/free per iteration */
    int16_t out[output_len];
    uint64_t hash = 0;

    const int il_minus_1 = input_len - 1;
    const int ol_minus_1 = output_len - 1;

    /* fixed-point step: 1.0 in 32.32 format */
    const uint64_t step = ((uint64_t)il_minus_1 << 32) / (uint64_t)ol_minus_1;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t pos = 0; /* 32.32 fixed point */

        /* bulk process 8 samples per inner trip */
        int i = 0;
        for (; i + 7 < output_len; i += 8) {
            for (int k = 0; k < 8; ++k) {
                uint32_t idx = (uint32_t)(pos >> 32);
                uint32_t frac = (uint32_t)(pos & 0xFFFFFFFFU); /* 0..2^32-1 */
                int left  = (idx < (uint32_t)il_minus_1) ? (int)idx : il_minus_1;
                int right = (left == il_minus_1) ? left : left + 1;

                /* 32-bit weighted interpolation without fp, rounded */
                int32_t v0 = src[left];
                int32_t v1 = src[right];
                int32_t v  = (int32_t)((((int64_t)(0x100000000ULL - frac) * v0 +
                                           (int64_t)frac * v1) + 0x80000000ULL) >> 32);
                out[i + k] = (int16_t)v;
                pos += step;
            }
        }
        /* tail < 8 samples */
        for (; i < output_len; ++i) {
            uint32_t idx  = (uint32_t)(pos >> 32);
            uint32_t frac = (uint32_t)(pos & 0xFFFFFFFFU);
            int left  = (idx < (uint32_t)il_minus_1) ? (int)idx : il_minus_1;
            int right = (left == il_minus_1) ? left : left + 1;
            int32_t v0 = src[left];
            int32_t v1 = src[right];
            int32_t v  = (int32_t)((((int64_t)(0x100000000ULL - frac) * v0 +
                                       (int64_t)frac * v1) + 0x80000000ULL) >> 32);
            out[i] = (int16_t)v;
            pos += step;
        }
        hash = checksum_i16(out, (size_t)output_len);
    }
    return hash;
}