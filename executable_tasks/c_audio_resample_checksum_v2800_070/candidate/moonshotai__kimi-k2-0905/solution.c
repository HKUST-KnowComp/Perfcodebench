#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    static int16_t* out = NULL;          /* reused buffer */
    static size_t   out_cap = 0;

    if (output_len > (int)out_cap) {
        out = (int16_t*)realloc(out, (size_t)output_len * sizeof(int16_t));
        if (!out) return 0;
        out_cap = (size_t)output_len;
    }

    /* fixed-point interpolation setup (1.30 format) */
    const int in_len_minus_1 = input_len - 1;
    const int out_len_minus_1 = output_len - 1;
    const uint32_t scale = ((uint32_t)1 << 30) / out_len_minus_1;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* resample --------------------------------------------------------- */
        uint32_t pos = 0;
        for (int i = 0; i < output_len; ++i) {
            int left  = (int)(pos >> 30);
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;
            uint32_t frac = pos & ((1u << 30) - 1);
            int32_t v = ((int32_t)src[left] * ((1u << 30) - frac) +
                         (int32_t)src[right] * frac) >> 30;
            out[i] = (int16_t)v;
            pos += scale * in_len_minus_1;
        }

        /* vectorized checksum --------------------------------------------- */
        const int16_t* p   = out;
        size_t n           = (size_t)output_len;
        uint64_t h0 = 1469598103934665603ULL;
        uint64_t h1 = 1469598103934665603ULL;
        uint64_t h2 = 1469598103934665603ULL;
        uint64_t h3 = 1469598103934665603ULL;

        while (n >= 8) {
            h0 ^= (uint16_t)p[0]; h0 *= 1099511628211ULL;
            h1 ^= (uint16_t)p[1]; h1 *= 1099511628211ULL;
            h2 ^= (uint16_t)p[2]; h2 *= 1099511628211ULL;
            h3 ^= (uint16_t)p[3]; h3 *= 1099511628211ULL;
            h0 ^= (uint16_t)p[4]; h0 *= 1099511628211ULL;
            h1 ^= (uint16_t)p[5]; h1 *= 1099511628211ULL;
            h2 ^= (uint16_t)p[6]; h2 *= 1099511628211ULL;
            h3 ^= (uint16_t)p[7]; h3 *= 1099511628211ULL;
            p += 8; n -= 8;
        }
        while (n--) {
            h0 ^= (uint16_t)(*p++); h0 *= 1099511628211ULL;
        }
        hash = h0 ^ h1 ^ h2 ^ h3;
    }
    return hash;
}