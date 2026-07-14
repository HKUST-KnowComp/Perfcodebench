#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* 32-bit fixed-point interpolation: 16 bits integer, 16 bits fraction */
typedef uint32_t fixed16;
#define FRAC_BITS 16
#define FRAC_ONE  (1U<<FRAC_BITS)
#define FRAC_MASK (FRAC_ONE-1)

static inline int16_t linear_interp_fixed(const int16_t* src, fixed16 pos_fixed,
                                          int input_len)
{
    int left  = (int)(pos_fixed >> FRAC_BITS);
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;
    uint32_t frac = pos_fixed & FRAC_MASK;          /* 0..0.FFFF */
    int32_t  a = src[left];
    int32_t  b = src[right];
    /* out = a + (b-a)*frac/65536  (rounded) */
    int32_t diff = b - a;
    int32_t prod = (diff * (int32_t)frac + (FRAC_ONE>>1)) >> FRAC_BITS;
    int32_t out  = a + prod;
    if (out > 32767)      return 32767;
    else if (out < -32768) return -32768;
    return (int16_t)out;
}

/* vectorised checksum: process 4×uint16 per iteration, fold to 64-bit */
static uint64_t checksum_i16_vec(const int16_t* data, size_t n)
{
    const uint64_t prime = 1099511628211ULL;
    uint64_t h[4] = { 1469598103934665603ULL,
                      1469598103934665603ULL,
                      1469598103934665603ULL,
                      1469598103934665603ULL };
    size_t i = 0;
    for (; i + 3 < n; i += 4) {
        h[0] ^= (uint16_t)data[i+0];  h[0] *= prime;
        h[1] ^= (uint16_t)data[i+1];  h[1] *= prime;
        h[2] ^= (uint16_t)data[i+2];  h[2] *= prime;
        h[3] ^= (uint16_t)data[i+3];  h[3] *= prime;
    }
    /* tail */
    for (; i < n; ++i) {
        h[0] ^= (uint16_t)data[i];
        h[0] *= prime;
    }
    /* fold into single hash */
    uint64_t hash = h[0];
    for (int k = 1; k < 4; ++k) {
        hash ^= h[k];
        hash *= prime;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters)
{
    /* allocate once, reuse every iteration */
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    /* precompute fixed-point step */
    const uint64_t num   = (uint64_t)(input_len - 1) << FRAC_BITS;
    const uint64_t den   = (uint64_t)(output_len - 1);
    const fixed16 step   = (fixed16)(num / den);   /* exact floor division */

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        fixed16 pos = 0;
        for (int i = 0; i < output_len; ++i) {
            out[i] = linear_interp_fixed(src, pos, input_len);
            pos += step;
        }
        hash = checksum_i16_vec(out, (size_t)output_len);
    }
    free(out);
    return hash;
}