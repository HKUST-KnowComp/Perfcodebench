#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const size_t n4 = n & ~(size_t)3;
    const uint16_t* u = (const uint16_t*)data;
    for (size_t i = 0; i < n4; i += 4) {
        hash ^= u[i];   hash *= 1099511628211ULL;
        hash ^= u[i+1]; hash *= 1099511628211ULL;
        hash ^= u[i+2]; hash *= 1099511628211ULL;
        hash ^= u[i+3]; hash *= 1099511628211ULL;
    }
    for (size_t i = n4; i < n; ++i) {
        hash ^= u[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 1 || input_len <= 1) return 0;

    const size_t len = (size_t)output_len;
    int16_t* out = (int16_t*)aligned_alloc(16, len * sizeof(int16_t));
    if (!out) return 0;

    const int ilm1 = input_len - 1;
    const int olm1 = output_len - 1;
    const int64_t ratio = ((int64_t)ilm1 << 20) / olm1;  /* Q20 */

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < output_len; ++i) {
            const int64_t pos = i * ratio;
            const int left  = (int)(pos >> 20);
            const int right = (left >= ilm1) ? ilm1 : left + 1;
            const int frac = (int)(pos & ((1<<20)-1));
            const int32_t a = src[left];
            const int32_t b = src[right];
            const int32_t v = (a * ((1<<20) - frac) + b * frac + (1<<19)) >> 20;
            out[i] = (int16_t)v;
        }
        hash = checksum_i16(out, len);
    }
    free(out);
    return hash;
}