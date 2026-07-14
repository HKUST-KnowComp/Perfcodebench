#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t *data, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        h ^= (uint16_t)data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const int16_t *src, int input_len, int output_len, int iters) {
    uint64_t hash = 0;
    const int64_t scale = 1LL << 32;
    const int64_t max_frac = scale - 1;
    const int64_t half = 1LL << 31;

    for (int iter = 0; iter < iters; ++iter) {
        int16_t *out = (int16_t *)malloc((size_t)output_len * sizeof(int16_t));
        if (!out) return 0;

        const int64_t step = ((int64_t)(input_len - 1) << 32) / (output_len - 1);
        int64_t pos = 0;

        int i = 0;
        for (; i + 3 < output_len; i += 4) {
            int idx0 = (int)(pos >> 32);
            int idx1 = idx0 + 1;
            if (idx1 >= input_len) idx1 = input_len - 1;
            int64_t frac = (pos & 0xFFFFFFFFLL);
            int64_t v0 = src[idx0];
            int64_t v1 = src[idx1];
            int64_t mix = v0 * (scale - frac) + v1 * frac;
            int32_t rounded = (int32_t)((mix + half) >> 32);
            out[i] = (int16_t)rounded;
            pos += step;

            idx0 = (int)(pos >> 32);
            idx1 = idx0 + 1;
            if (idx1 >= input_len) idx1 = input_len - 1;
            frac = (pos & 0xFFFFFFFFLL);
            v0 = src[idx0];
            v1 = src[idx1];
            mix = v0 * (scale - frac) + v1 * frac;
            rounded = (int32_t)((mix + half) >> 32);
            out[i+1] = (int16_t)rounded;
            pos += step;

            idx0 = (int)(pos >> 32);
            idx1 = idx0 + 1;
            if (idx1 >= input_len) idx1 = input_len - 1;
            frac = (pos & 0xFFFFFFFFLL);
            v0 = src[idx0];
            v1 = src[idx1];
            mix = v0 * (scale - frac) + v1 * frac;
            rounded = (int32_t)((mix + half) >> 32);
            out[i+2] = (int16_t)rounded;
            pos += step;

            idx0 = (int)(pos >> 32);
            idx1 = idx0 + 1;
            if (idx1 >= input_len) idx1 = input_len - 1;
            frac = (pos & 0xFFFFFFFFLL);
            v0 = src[idx0];
            v1 = src[idx1];
            mix = v0 * (scale - frac) + v1 * frac;
            rounded = (int32_t)((mix + half) >> 32);
            out[i+3] = (int16_t)rounded;
            pos += step;
        }
        for (; i < output_len; ++i) {
            int idx0 = (int)(pos >> 32);
            int idx1 = idx0 + 1;
            if (idx1 >= input_len) idx1 = input_len - 1;
            int64_t frac = (pos & 0xFFFFFFFFLL);
            int64_t v0 = src[idx0];
            int64_t v1 = src[idx1];
            int64_t mix = v0 * (scale - frac) + v1 * frac;
            int32_t rounded = (int32_t)((mix + half) >> 32);
            out[i] = (int16_t)rounded;
            pos += step;
        }

        hash = checksum_i16(out, (size_t)output_len);
        free(out);
    }
    return hash;
}