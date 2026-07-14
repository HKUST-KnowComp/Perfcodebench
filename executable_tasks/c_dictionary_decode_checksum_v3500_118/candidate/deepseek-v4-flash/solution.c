#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    uint64_t h;
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;
    for (int iter = 0; iter < iters; ++iter) {
        h = fnv_offset;
        for (size_t i = 0; i < n; ++i) {
            h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
            h *= fnv_prime;
        }
    }
    return h;
}
