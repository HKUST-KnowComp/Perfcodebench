#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    const int32_t* data = logits.data();
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const int32_t* row_ptr = data;
        for (int t = 0; t < tokens; ++t) {
            int32_t v0 = row_ptr[0];
            int32_t v1 = row_ptr[1];
            
            int32_t max1_val, max1_idx;
            int32_t max2_val, max2_idx;
            
            if (v0 >= v1) {
                max1_val = v0; max1_idx = 0;
                max2_val = v1; max2_idx = 1;
            } else {
                max1_val = v1; max1_idx = 1;
                max2_val = v0; max2_idx = 0;
            }
            
            for (int e = 2; e < experts; ++e) {
                int32_t val = row_ptr[e];
                if (val > max1_val) {
                    max2_val = max1_val; max2_idx = max1_idx;
                    max1_val = val; max1_idx = e;
                } else if (val > max2_val) {
                    max2_val = val; max2_idx = e;
                }
            }
            
            hash ^= static_cast<uint32_t>(max1_val);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint32_t>(max1_idx);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint32_t>(max2_val);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint32_t>(max2_idx);
            hash *= 1099511628211ULL;
            
            row_ptr += experts;
        }
    }
    return hash;
}
