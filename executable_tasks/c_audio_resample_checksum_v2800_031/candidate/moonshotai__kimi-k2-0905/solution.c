#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t *data, size_t n)
{
    uint64_t h = 14695981039346656037ULL;
    for (size_t i = 0; i < n; i++) {
        h ^= (uint16_t)data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const int16_t *src, int ilen, int olen, int iters)
{
    /* precompute fixed-point constants once */
    uint32_t N = (uint32_t)(ilen - 1);
    uint32_t M = (uint32_t)(olen - 1);
    /* rescale factor in 16.16 fixed-point so we can do integer work from here on */
    uint32_t step = (N << 16) / M;

    /* allocate once; keep across iterations */
    int16_t *out = (int16_t *)malloc((size_t)olen * sizeof(int16_t));
    if (!out) return 0;

    uint64_t h = 0;
    for (int it = 0; it < iters; it++) {
        /* for every output index i compute pos = i*step in fixed-point 16.16 */
        for (int i = 0; i < olen; i++) {
            uint32_t p  = (uint32_t)i * step;
            uint32_t idx = p >> 16;          /* integer part (left index) */
            uint32_t frac = p & 0xFFFFUL;    /* fractional part in [0, 65535] */
            uint32_t w0 = frac;
            uint32_t w1 = 65535U - frac;

            int16_t v0 = src[idx];
            int16_t v1 = src[idx < (uint32_t)(ilen - 1) ? idx + 1 : (uint32_t)(ilen - 1)];
            /* 16-bit fixed-point lerp: result = (w0*v0 + w1*v1 + 32768) >> 16 */
            uint32_t t = w0 * (uint16_t)v0 + w1 * (uint16_t)v1 + 32768;
            out[i] = (int16_t)(t >> 16);
        }
        h = checksum_i16(out, (size_t)olen);
    }
    /* single free at the end */
    free(out);
    return h;
}