#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (input_len <= 0 || output_len <= 0 || iters <= 0) return 0;

    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    const int64_t step = ((int64_t)(input_len - 1) << 32) / (output_len - 1);
    const int64_t half = 1LL << 31;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t pos = 0;
        int i = 0;

        /* unroll 4x */
        for (; i + 4 <= output_len; i += 4) {
            for (int k = 0; k < 4; ++k) {
                int idx = (int)(pos >> 32);
                int idx2 = idx + 1;
                if (idx2 >= input_len) idx2 = input_len - 1;

                uint32_t frac = (uint32_t)(pos & 0xFFFFFFFFU);
                int32_t v1 = src[idx];
                int32_t v2 = src[idx2];

                /* 32-bit fixed-point linear interpolation */
                int32_t val = v1 + (int32_t)(((int64_t)(v2 - v1) * frac) >> 32);
                if (val > 32767) val = 32767;
                else if (val < -32768) val = -32768;
                out[i + k] = (int16_t)val;
                pos += step;
            }
        }

        /* tail */
        for (; i < output_len; ++i) {
            int idx = (int)(pos >> 32);
            int idx2 = idx + 1;
            if (idx2 >= input_len) idx2 = input_len - 1;

            uint32_t frac = (uint32_t)(pos & 0xFFFFFFFFU);
            int32_t v1 = src[idx];
            int32_t v2 = src[idx2];

            int32_t val = v1 + (int32_t)(((int64_t)(v2 - v1) * frac) >> 32);
            if (val > 32767) val = 32767;
            else if (val < -32768) val = -32768;
            out[i] = (int16_t)val;
            pos += step;
        }

        hash = checksum_i16(out, (size_t)output_len);
    }

    free(out);
    return hash;
}