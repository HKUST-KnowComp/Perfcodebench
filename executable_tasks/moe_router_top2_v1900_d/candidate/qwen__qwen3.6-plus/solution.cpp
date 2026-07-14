#include "interface.h"
#include <cstdint>
#include <climits>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    uint64_t hash = 0;
    const int32_t* data = logits.data();
    const std::size_t experts_sz = static_cast<std::size_t>(experts);
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const int32_t* row = data;
        for (int t = 0; t < tokens; ++t) {
            int32_t b1_val = INT32_MIN;
            int b1_idx = experts;
            int32_t b2_val = INT32_MIN;
            int b2_idx = experts;
            
            for (int e = 0; e < experts; ++e) {
                int32_t val = row[e];
                if (val > b1_val || (val == b1_val && e < b1_idx)) {
                    b2_val = b1_val;
                    b2_idx = b1_idx;
                    b1_val = val;
                    b1_idx = e;
                } else if (val > b2_val || (val == b2_val && e < b2_idx)) {
                    b2_val = val;
                    b2_idx = e;
                }
            }
            
            hash ^= static_cast<uint64_t>(static_cast<uint32_t>(b1_val));
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(b1_idx);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(static_cast<uint32_t>(b2_val));
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(b2_idx);
            hash *= 1099511628211ULL;
            
            row += experts_sz;
        }
    }
    return hash;
}