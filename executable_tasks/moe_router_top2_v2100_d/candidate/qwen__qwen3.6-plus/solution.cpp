#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <utility>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    (void)iters;
    const int32_t* data = logits.data();
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    uint64_t hash = FNV_OFFSET;
    
    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = data + static_cast<std::size_t>(t) * experts;
        
        int32_t v1 = row[0];
        int i1 = 0;
        int32_t v2 = row[1];
        int i2 = 1;
        
        if (v2 > v1) {
            std::swap(v1, v2);
            std::swap(i1, i2);
        }
        
        for (int e = 2; e < experts; ++e) {
            int32_t val = row[e];
            if (val > v1 || (val == v1 && e < i1)) {
                v2 = v1; i2 = i1;
                v1 = val; i1 = e;
            } else if (val > v2 || (val == v2 && e < i2)) {
                v2 = val; i2 = e;
            }
        }
        
        hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v1));
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(i1);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v2));
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(i2);
        hash *= FNV_PRIME;
    }
    
    return hash;
}