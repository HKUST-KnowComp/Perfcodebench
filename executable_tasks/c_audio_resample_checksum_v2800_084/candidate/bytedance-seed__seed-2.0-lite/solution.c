#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (input_len < 1 || output_len < 1 || iters < 1) return 0;

    typedef struct {
        int left;
        int right;
        double frac;
        double one_minus_frac;
    } ResamplePos;

    ResamplePos* pos = (ResamplePos*)malloc((size_t)output_len * sizeof(ResamplePos));
    if (!pos) return 0;

    const double scale = (double)(input_len - 1) / (double)(output_len - 1);
    for (int i = 0; i < output_len; ++i) {
        const double p = (double)i * scale;
        const int l = (int)p;
        int r = l + 1;
        if (r >= input_len) r = input_len - 1;
        const double frac = p - (double)l;
        pos[i].left = l;
        pos[i].right = r;
        pos[i].frac = frac;
        pos[i].one_minus_frac = 1.0 - frac;
    }

    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        for (int i = 0; i < output_len; ++i) {
            const ResamplePos* p = &pos[i];
            const double sample = (double)src[p->left] * p->one_minus_frac + (double)src[p->right] * p->frac;
            int16_t s;
            if (sample >= 0.0) {
                s = (int16_t)(sample + 0.5);
            } else {
                s = (int16_t)(sample - 0.5);
            }
            hash ^= (uint16_t)s;
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    free(pos);
    return final_hash;
}