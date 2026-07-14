#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;

    const uint64_t kMul = 1099511628211ULL;
    const uint64_t kInit = 1469598103934665603ULL;

    uint64_t h = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash0 = kInit;
        size_t i = 0;

        /* Manual 4-way unroll to expose ILP */
        if (n >= 4) {
            uint64_t hash1 = kInit;
            uint64_t hash2 = kInit;
            uint64_t hash3 = kInit;

            for (; i + 3 < n; i += 4) {
                uint32_t v0 = dict_vals[ids[i + 0]];
                uint32_t v1 = dict_vals[ids[i + 1]];
                uint32_t v2 = dict_vals[ids[i + 2]];
                uint32_t v3 = dict_vals[ids[i + 3]];

                hash0 ^= (uint64_t)v0 + 1ULL;
                hash1 ^= (uint64_t)v1 + 1ULL;
                hash2 ^= (uint64_t)v2 + 1ULL;
                hash3 ^= (uint64_t)v3 + 1ULL;

                hash0 *= kMul;
                hash1 *= kMul;
                hash2 *= kMul;
                hash3 *= kMul;
            }

            /* Combine partial hashes */
            hash0 ^= hash1;
            hash0 *= kMul;
            hash0 ^= hash2;
            hash0 *= kMul;
            hash0 ^= hash3;
            hash0 *= kMul;
            h = hash0;
        } else {
            h = kInit;
        }

        /* Tail */
        for (; i < n; ++i) {
            uint32_t v = dict_vals[ids[i]];
            h ^= (uint64_t)v + 1ULL;
            h *= kMul;
        }
    }

    return h;
}