#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstddef>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                          const std::vector<uint32_t>& values,
                          int groups,
                          int iters) {
    // Dense aggregation: one slot per group.
    std::vector<uint64_t> sums(groups, 0);

    // Raw pointers for faster indexing.
    const uint32_t* keys_ptr = keys.data();
    const uint32_t* values_ptr = values.data();
    std::size_t n = keys.size();

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset accumulator.
        std::fill(sums.begin(), sums.end(), 0);

        // Aggregate.
        for (std::size_t i = 0; i < n; ++i) {
            sums[keys_ptr[i]] += static_cast<uint64_t>(values_ptr[i]);
        }

        // Compute FNV-1a checksum over all groups.
        hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums[g];
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}
