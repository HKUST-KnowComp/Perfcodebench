#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> agg(static_cast<size_t>(groups), 0);
    uint64_t out = 0;
    const size_t n_elements = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Accumulate values into contiguous group array for optimal cache/vectorization
        for (size_t i = 0; i < n_elements; ++i) {
            agg[static_cast<size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }

        // Compute checksum and reset accumulator in a single pass to avoid separate zero-fill
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            const uint64_t v = agg[static_cast<size_t>(g)];
            hash ^= v;
            hash *= 1099511628211ULL;
            agg[static_cast<size_t>(g)] = 0;
        }
        out = hash;
    }
    return out;
}