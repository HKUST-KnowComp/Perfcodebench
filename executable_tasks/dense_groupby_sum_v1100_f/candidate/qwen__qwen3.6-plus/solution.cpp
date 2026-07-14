#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups);
    uint64_t out = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(sums.begin(), sums.end(), 0ULL);
        
        for (size_t i = 0; i < keys.size(); ++i) {
            sums[keys[i]] += static_cast<uint64_t>(values[i]);
        }
        
        uint64_t hash = FNV_OFFSET;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums[g];
            hash *= FNV_PRIME;
        }
        out = hash;
    }
    return out;
}