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
    /* trivial case */
    if (output_len <= 1 || input_len <= 1) return 0;

    /* allocate output buffer once */
    int16_t* __restrict out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    /* pre-compute fixed position table once (outside the iteration loop) */
    uint32_t* __restrict idx = (uint32_t*)malloc((size_t)output_len * sizeof(uint32_t));
    if (!idx) { free(out); return 0; }

    const double scale = (double)(input_len - 1) / (double)(output_len - 1);
    for (int i = 0; i < output_len; ++i) {
        double pos = (double)i * scale;
        uint32_t left  = (uint32_t)pos;
        uint32_t right = left + 1;
        if (right >= (uint32_t)input_len) right = (uint32_t)input_len - 1;
        idx[i] = left | (right << 16);          /* pack two 16-bit indices */
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* vectorisable, branch-free resampling loop */
        for (int i = 0; i < output_len; ++i) {
            uint32_t lr = idx[i];
            int left  =  lr        & 0xFFFF;
            int right = (lr >> 16) & 0xFFFF;

            int32_t s_left  = src[left];
            int32_t s_right = src[right];

            /* fixed-point fraction: 16 LSB hold 0..65535 representing 0..1 */
            uint32_t frac_u = (uint32_t)(((double)i * scale - (double)left) * 65536.0 + 0.5);
            if (frac_u > 65535) frac_u = 65535;

            /* linear interpolation in 17.15 fixed point */
            int32_t val = (s_left  * (65536 - (int32_t)frac_u) +
                           s_right * (int32_t)frac_u + 16384) >> 15;

            /* saturate and round to int16_t */
            if (val >  32767) val =  32767;
            if (val < -32768) val = -32768;
            out[i] = (int16_t)val;
        }
        hash = checksum_i16(out, (size_t)output_len);
    }

    free(idx);
    free(out);
    return hash;
}