#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    uint64_t h = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t* id_ptr = ids;
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            uint64_t v0 = dict_vals[id_ptr[i]] + 1ULL;
            uint64_t v1 = dict_vals[id_ptr[i+1]] + 1ULL;
            uint64_t v2 = dict_vals[id_ptr[i+2]] + 1ULL;
            uint64_t v3 = dict_vals[id_ptr[i+3]] + 1ULL;
            h ^= v0; h *= 1099511628211ULL;
            h ^= v1; h *= 1099511628211ULL;
            h ^= v2; h *= 1099511628211ULL;
            h ^= v3; h *= 1099511628211ULL;
        }
        for (; i < n; ++i) {
            h ^= (uint64_t)dict_vals[id_ptr[i]] + 1ULL;
            h *= 1099511628211ULL;
        }
    }
    return h;
}