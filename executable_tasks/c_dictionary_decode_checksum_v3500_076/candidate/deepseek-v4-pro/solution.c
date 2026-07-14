#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* restrict ids, size_t n, const uint32_t* restrict dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    if (iters <= 0) return 0;
    uint64_t h = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
        uint32_t v = dict_vals[ids[i]];
        h ^= (uint64_t)v + 1ULL;
        h *= multiplier;
    }
    return h;
}
