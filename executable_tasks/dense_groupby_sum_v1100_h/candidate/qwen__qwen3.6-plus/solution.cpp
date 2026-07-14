#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups, 0);
    uint64_t out = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    size_t n = keys.size();
    uint64_t* s_ptr = sums.data();
    size_t g_count = static_cast<size_t>(groups);
    
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(s_ptr, s_ptr + g_count, 0ULL);
        
        for (size_t i = 0; i < n; ++i) {
            s_ptr[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }
        
        uint64_t hash = FNV_OFFSET;
        for (size_t g = 0; g < g_count; ++g) {
            hash ^= s_ptr[g];
            hash *= FNV_PRIME;
        }
        out = hash;
    }
    return out;
}
