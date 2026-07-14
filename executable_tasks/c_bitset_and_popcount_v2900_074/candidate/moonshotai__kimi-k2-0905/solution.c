#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/* portable popcnt intrinsic */
static inline uint64_t popcnt_u64(uint64_t x) {
    return __builtin_popcountll(x);
}

/* process 4 words per iteration */
static inline void popcnt_and_store(uint64_t *dst,
                                    const uint64_t *a,
                                    const uint64_t *b)
{
    dst[0] = a[0] & b[0];
    dst[1] = a[1] & b[1];
    dst[2] = a[2] & b[2];
    dst[3] = a[3] & b[3];
}

uint64_t run(const uint64_t *left,
             const uint64_t *right,
             size_t word_count,
             int iters)
{
    const size_t CHUNK = 4;
    const size_t full = word_count / CHUNK;
    const size_t rest = word_count % CHUNK;

    uint64_t acc = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;

        const uint64_t *l = left;
        const uint64_t *r = right;

        /* vectorised 4-word popcnt */
        for (size_t i = 0; i < full; i++) {
            uint64_t v[4];
            popcnt_and_store(v, l, r);
            local += popcnt_u64(v[0]) + popcnt_u64(v[1]) +
                     popcnt_u64(v[2]) + popcnt_u64(v[3]);
            l += CHUNK;
            r += CHUNK;
        }

        /* tail scalar */
        for (size_t i = 0; i < rest; i++)
            local += popcnt_u64(*l++ & *r++);

        acc = local;
    }
    return acc;
}