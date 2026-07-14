#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (input_len <= 1 || output_len <= 1) {
        /* degenerate cases handled as before to preserve correctness */
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
            if (!out) return 0;
            for (int i = 0; i < output_len; ++i) {
                double pos = (double)i * (double)(input_len - 1) / (double)(output_len - 1);
                int left = (int)pos;
                int right = left + 1;
                if (right >= input_len) right = input_len - 1;
                double frac = pos - (double)left;
                double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
                out[i] = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));
            }
            hash = checksum_i16(out, (size_t)output_len);
            free(out);
        }
        return hash;
    }

    /* fast path: fixed-point linear interpolation with 32 fractional bits */
    const int64_t scale = ((int64_t)(input_len - 1) << 32) / (output_len - 1);

    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < output_len; ++i) {
            int64_t pos = i * scale;
            int left = (int)(pos >> 32);
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;
            uint32_t frac = (uint32_t)pos;   /* fractional part: 0 … 2^32-1 */
            int32_t v1 = src[left];
            int32_t v2 = src[right];
            int32_t sample = ((int64_t)v1 * ((1ULL << 32) - frac) +
                              (int64_t)v2 * frac + (1ULL << 31)) >> 32;
            out[i] = (int16_t)sample;
        }
        hash = checksum_i16(out, (size_t)output_len);
    }

    free(out);
    return hash;
}