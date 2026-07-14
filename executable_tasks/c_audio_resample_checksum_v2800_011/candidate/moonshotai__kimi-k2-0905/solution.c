#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    /* allocate once, reuse every iteration */
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    /* precompute fixed-point constants */
    const int in_last  = input_len - 1;
    const int out_last = output_len - 1;
    const uint32_t scale = (uint32_t)in_last;
    const uint32_t norm  = (uint32_t)out_last;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int i = 0;
        /* process 4 samples per iteration */
        for (; i + 3 < output_len; i += 4) {
            uint32_t pos0 = ((uint32_t)i       * scale + (norm >> 1)) / norm;
            uint32_t pos1 = ((uint32_t)(i + 1) * scale + (norm >> 1)) / norm;
            uint32_t pos2 = ((uint32_t)(i + 2) * scale + (norm >> 1)) / norm;
            uint32_t pos3 = ((uint32_t)(i + 3) * scale + (norm >> 1)) / norm;

            int left0 = (int)pos0, right0 = left0 + 1; if (right0 >= input_len) right0 = in_last;
            int left1 = (int)pos1, right1 = left1 + 1; if (right1 >= input_len) right1 = in_last;
            int left2 = (int)pos2, right2 = left2 + 1; if (right2 >= input_len) right2 = in_last;
            int left3 = (int)pos3, right3 = left3 + 1; if (right3 >= input_len) right3 = in_last;

            uint32_t frac0 = ((uint32_t)i       * scale) % norm;
            uint32_t frac1 = ((uint32_t)(i + 1) * scale) % norm;
            uint32_t frac2 = ((uint32_t)(i + 2) * scale) % norm;
            uint32_t frac3 = ((uint32_t)(i + 3) * scale) % norm;

            int32_t v0 = (int32_t)src[left0] * (int32_t)(norm - frac0) +
                         (int32_t)src[right0] * (int32_t)frac0;
            int32_t v1 = (int32_t)src[left1] * (int32_t)(norm - frac1) +
                         (int32_t)src[right1] * (int32_t)frac1;
            int32_t v2 = (int32_t)src[left2] * (int32_t)(norm - frac2) +
                         (int32_t)src[right2] * (int32_t)frac2;
            int32_t v3 = (int32_t)src[left3] * (int32_t)(norm - frac3) +
                         (int32_t)src[right3] * (int32_t)frac3;

            /* round and shift */
            v0 = (v0 + (norm >> 1)) / (int32_t)norm;
            v1 = (v1 + (norm >> 1)) / (int32_t)norm;
            v2 = (v2 + (norm >> 1)) / (int32_t)norm;
            v3 = (v3 + (norm >> 1)) / (int32_t)norm;

            out[i]     = (int16_t)v0;
            out[i + 1] = (int16_t)v1;
            out[i + 2] = (int16_t)v2;
            out[i + 3] = (int16_t)v3;
        }
        /* tail */
        for (; i < output_len; ++i) {
            uint32_t pos  = ((uint32_t)i * scale + (norm >> 1)) / norm;
            int left  = (int)pos;
            int right = left + 1; if (right >= input_len) right = in_last;
            uint32_t frac = ((uint32_t)i * scale) % norm;
            int32_t v = (int32_t)src[left]  * (int32_t)(norm - frac) +
                        (int32_t)src[right] * (int32_t)frac;
            v = (v + (norm >> 1)) / (int32_t)norm;
            out[i] = (int16_t)v;
        }
        hash = checksum_i16(out, (size_t)output_len);
    }
    free(out);
    return hash;
}