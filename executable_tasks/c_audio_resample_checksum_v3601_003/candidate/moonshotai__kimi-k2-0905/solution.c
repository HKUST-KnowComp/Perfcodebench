#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    size_t i = 0;
    if (n >= 4) {
        for (; i < n - 3; i += 4) {
            h ^= (uint16_t)data[i];
            h *= 1099511628211ULL;
            h ^= (uint16_t)data[i+1];
            h *= 1099511628211ULL;
            h ^= (uint16_t)data[i+2];
            h *= 1099511628211ULL;
            h ^= (uint16_t)data[i+3];
            h *= 1099511628211ULL;
        }
    }
    for (; i < n; ++i) {
        h ^= (uint16_t)data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (input_len <= 0 || output_len <= 0 || iters <= 0) return 0;
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    const int64_t ratio_num = ((int64_t)(input_len - 1)) << 32;
    const int64_t ratio_den = output_len - 1;

    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < output_len; ++i) {
            int64_t pos_fp = (i * ratio_num) / ratio_den;
            int left = (int)(pos_fp >> 32);
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;
            uint32_t frac = (uint32_t)pos_fp;

            int32_t l = src[left];
            int32_t r = src[right];
            int32_t sample = (l * ((1ULL << 32) - frac) + r * frac + (1ULL << 31)) >> 32;
            out[i] = (int16_t)sample;
        }
        hash = checksum_i16(out, (size_t)output_len);
    }
    free(out);
    return hash;
}