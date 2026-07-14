#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t *data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t *src, int input_len, int output_len, int iters) {
    uint64_t hash = 0;
    const int outlen = output_len;
    const int inlen1  = input_len - 1;
    const int outlen1 = output_len - 1;

    for (int iter = 0; iter < iters; ++iter) {
        int16_t *out = (int16_t *)malloc((size_t)outlen * sizeof(int16_t));
        if (!out) return 0;

        /* fixed-point scale: 1.0  -> (1<<16) */
        const uint32_t scale = 65536u;
        const uint32_t half  = 32768u;   /* 0.5 in fixed point */

        for (int i = 0; i < outlen; ++i) {
            /* pos = i * (inlen1) / (outlen1)   as 32-bit fixed point */
            uint32_t pos = (uint32_t)i * (uint32_t)inlen1;
            uint32_t frac = (pos % (uint32_t)outlen1) * scale / (uint32_t)outlen1;
            int left  = pos / (uint32_t)outlen1;
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            int32_t sleft  = src[left];
            int32_t sright = src[right];
            /* sample = sleft*(scale-frac) + sright*frac  */
            int32_t v = sleft * (scale - frac) + sright * frac;
            /* round to nearest and shift back */
            v = (v + half) >> 16;
            /* saturate to int16 range */
            if (v >  32767) v =  32767;
            if (v < -32768) v = -32768;
            out[i] = (int16_t)v;
        }

        hash = checksum_i16(out, (size_t)outlen);
        free(out);
    }
    return hash;
}