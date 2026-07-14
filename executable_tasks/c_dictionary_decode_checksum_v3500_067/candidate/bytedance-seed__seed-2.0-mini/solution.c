#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    uint64_t h;
    const uint32_t* restrict ids_restrict = ids;
    const uint32_t* restrict dict_restrict = dict_vals;
    for (int iter = 0; iter < iters; ++iter) {
        h = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            const uint32_t val = dict_restrict[ids_restrict[i]];
            h ^= (uint64_t)val + 1ULL;
            h *= 1099511628211ULL;
        }
    }
    return h;
}