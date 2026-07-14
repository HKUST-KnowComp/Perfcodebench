#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* 32-bit fixed-point: 16 bits integer, 16 bits fractional */
typedef uint32_t fixed16;
static const int FRAC_BITS = 16;
static const fixed16 FRAC_MASK = 0xFFFFU;

static inline int16_t lerp16(const int16_t *src, fixed16 pos, int len) {
    int idx = pos >> FRAC_BITS;
    int frac = pos & FRAC_MASK;
    int32_t a = src[idx];
    int32_t b = src[(idx + 1 < len) ? idx + 1 : idx];
    int32_t v = a * (0x10000 - frac) + b * frac;
    return (int16_t)((v + 0x8000) >> FRAC_BITS);
}

static uint64_t checksum_i16_vec(const int16_t *data, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    size_t i = 0;
    /* process 4 samples at a time */
    for (; i + 4 <= n; i += 4) {
        uint64_t chunk = 0;
        memcpy(&chunk, &data[i], 8); /* little-endian load */
        h ^= chunk;
        h *= 1099511628211ULL;
    }
    /* tail */
    for (; i < n; ++i) {
        h ^= (uint16_t)data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const int16_t *src, int input_len, int output_len, int iters) {
    /* allocate once, reuse every iteration */
    int16_t *out = (int16_t *)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    /* precompute fixed-point step */
    const fixed16 step = (uint64_t)(input_len - 1) << FRAC_BITS;
    const fixed16 step_frac = (uint64_t)(step) / (output_len - 1);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        fixed16 pos = 0;
        for (int i = 0; i < output_len; ++i) {
            out[i] = lerp16(src, pos, input_len);
            pos += step_frac;
        }
        hash = checksum_i16_vec(out, (size_t)output_len);
    }
    free(out);
    return hash;
}