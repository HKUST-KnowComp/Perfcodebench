#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_h = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            uint32_t val = dict_vals[ids[i]];
            local_h ^= (uint64_t)val + 1ULL;
            local_h *= 1099511628211ULL;
        }
        h = local_h;
    }
    return h;
}
