#include "interface.h"
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    uint64_t out = 0;
    if (groups <= 0 || iters <= 0) {
        return out;
    }
    std::vector<uint64_t> sums(static_cast<size_t>(groups), 0ULL);
    const uint64_t fnv_prime = 1099511628211ULL;
    const uint64_t fnv_offset_basis = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        for (int g = 0; g < groups; ++g) {
            sums[g] = 0;
        }
        const size_t count = keys.size();
        for (size_t i = 0; i < count; ++i) {
            const uint32_t key = keys[i];
            sums[key] += static_cast<uint64_t>(values[i]);
        }
        uint64_t hash = fnv_offset_basis;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums[g];
            hash *= fnv_prime;
        }
        out = hash;
    }
    return out;
}