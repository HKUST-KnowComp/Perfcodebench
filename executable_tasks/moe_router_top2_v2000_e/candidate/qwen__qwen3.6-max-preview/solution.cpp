#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstddef>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    (void)iters;
    const int32_t* data = logits.data();
    uint64_t hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
        
        int32_t v1 = row[0];
        int idx1 = 0;
        int32_t v2 = row[1];
        int idx2 = 1;
        
        if (v2 > v1) {
            v1 = row[1]; idx1 = 1;
            v2 = row[0]; idx2 = 0;
        }
        
        for (int e = 2; e < experts; ++e) {
            int32_t val = row[e];
            if (val > v1) {
                v2 = v1; idx2 = idx1;
                v1 = val; idx1 = e;
            } else if (val > v2) {
                v2 = val; idx2 = e;
            }
        }
        
        hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v1));
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(idx1);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v2));
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(idx2);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}