#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t calculate_checksum(const std::vector<uint64_t>& agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        uint64_t v = agg[g];
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 0;
    
    // Use a vector for O(1) direct indexing instead of a hash map.
    // This leverages the 'dense' nature of the keys and the known 'groups' bound.
    std::vector<uint64_t> agg(groups, 0);
    uint64_t last_hash = 0;
    const std::size_t n = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation buffer
        std::fill(agg.begin(), agg.end(), 0);

        // Perform aggregation
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t key = keys[i];
            // Safety check: only aggregate if key is within the valid group range
            if (key < static_cast<uint32_t>(groups)) {
                agg[key] += static_cast<uint64_t>(values[i]);
            }
        }
        
        last_hash = calculate_checksum(agg, groups);
    }

    return last_hash;
}