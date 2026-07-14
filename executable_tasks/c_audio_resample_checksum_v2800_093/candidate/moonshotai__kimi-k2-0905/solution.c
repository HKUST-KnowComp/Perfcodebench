#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_i16(const int16_t* __restrict data, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    const uint16_t* d = (const uint16_t*)data;
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        h ^= d[i];   h *= 1099511628211ULL;
        h ^= d[i+1]; h *= 1099511628211ULL;
        h ^= d[i+2]; h *= 1099511628211ULL;
        h ^= d[i+3]; h *= 1099511628211ULL;
    }
    for (; i < n; ++i) {
        h ^= d[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const int16_t* __restrict src, int input_len, int output_len, int iters) {
    int16_t* __restrict out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    const int64_t step = ((int64_t)(input_len - 1) << 32) / (output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
        int64_t pos = 0;
        int i = 0;

        /* Unroll 4x */
        for (; i + 4 <= output_len; i += 4) {
            int idx0 = (int)(pos >> 32);
            int idx1 = (int)((pos + 1*step) >> 32);
            int idx2 = (int)((pos + 2*step) >> 32);
            int idx3 = (int)((pos + 3*step) >> 32);

            int frac0 = (int)((pos >> 16) & 0xFFFF);
            int frac1 = (int)(((pos + 1*step) >> 16) & 0xFFFF);
            int frac2 = (int)(((pos + 2*step) >> 16) & 0xFFFF);
            int frac3 = (int)(((pos + 3*step) >> 16) & 0xFFFF);

            int32_t v00 = src[idx0], v01 = src[idx0+1];
            int32_t v10 = src[idx1], v11 = src[idx1+1];
            int32_t v20 = src[idx2], v21 = src[idx2+1];
            int32_t v30 = src[idx3], v31 = src[idx3+1];

            out[i+0] = (int16_t)((v00 * (65536 - frac0) + v01 * frac0 + 32768) >> 16);
            out[i+1] = (int16_t)((v10 * (65536 - frac1) + v11 * frac1 + 32768) >> 16);
            out[i+2] = (int16_t)((v20 * (65536 - frac2) + v21 * frac2 + 32768) >> 16);
            out[i+3] = (int16_t)((v30 * (65536 - frac3) + v31 * frac3 + 32768) >> 16);

            pos += 4*step;
        }
        /* Tail */
        for (; i < output_len; ++i) {
            int idx  = (int)(pos >> 32);
            int frac = (int)((pos >> 16) & 0xFFFF);
            int32_t v0 = src[idx], v1 = src[idx+1];
            out[i] = (int16_t)((v0 * (65536 - frac) + v1 * frac + 32768) >> 16);
            pos += step;
        }

        hash = checksum_i16(out, (size_t)output_len);
    }
    free(out);
    return hash;
}