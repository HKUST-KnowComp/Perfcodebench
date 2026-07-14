#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(static_cast<size_t>(groups), 0);
    const uint64_t fnv_prime = 1099511628211ULL;
    uint64_t out = 0;
    const size_t num_elements = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < num_elements; ++i) {
            const uint32_t key = keys[i];
            sums[key] += static_cast<uint64_t>(values[i]);
        }

        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            const uint64_t v = sums[g];
            hash ^= v;
            hash *= fnv_prime;
            sums[g] = 0;
        }
        out = hash;
    }

    return out;
}