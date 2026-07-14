#include "interface.h"
#include <vector>
#include <cstring>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> agg(groups);
    uint64_t out = 0;
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    const size_t agg_bytes = static_cast<size_t>(groups) * sizeof(uint64_t);
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(agg.data(), 0, agg_bytes);
        
        for (size_t i = 0; i < n; ++i) {
            agg[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }
        
        uint64_t hash = 1469598103934665603ULL;
        const uint64_t* a_ptr = agg.data();
        for (int g = 0; g < groups; ++g) {
            hash ^= a_ptr[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}