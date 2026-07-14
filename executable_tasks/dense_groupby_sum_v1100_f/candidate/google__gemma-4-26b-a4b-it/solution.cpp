#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t compute_checksum(const std::vector<uint64_t>& agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= agg[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 1469598103934665603ULL;
    
    std::vector<uint64_t> agg(groups);
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset aggregation array
        std::fill(agg.begin(), agg.end(), 0);

        // Perform aggregation using direct indexing
        for (size_t i = 0; i < n; ++i) {
            agg[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }

        last_hash = compute_checksum(agg, groups);
    }

    return last_hash;
}