#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    const uint64_t prime = 1099511628211ULL;
    const uint64_t offset = 1469598103934665603ULL;

    uint64_t h = offset;
    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t* id = ids;
        const uint32_t* end = ids + n;
        uint64_t local = h;

        /* unrolled hot loop: decode + hash fused */
        while (end - id >= 4) {
            uint64_t v0 = dict_vals[id[0]] + 1ULL;
            uint64_t v1 = dict_vals[id[1]] + 1ULL;
            uint64_t v2 = dict_vals[id[2]] + 1ULL;
            uint64_t v3 = dict_vals[id[3]] + 1ULL;

            local ^= v0; local *= prime;
            local ^= v1; local *= prime;
            local ^= v2; local *= prime;
            local ^= v3; local *= prime;
            id += 4;
        }
        while (id != end) {
            local ^= (uint64_t)(dict_vals[*id] + 1ULL);
            local *= prime;
            ++id;
        }
        h = local;
    }
    return h;
}