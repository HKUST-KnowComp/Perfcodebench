#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    const int32_t* data = logits.data();
    uint64_t final_hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (int t = 0; t < tokens; ++t) {
            const int32_t* row = data + t * experts;
            
            int i1 = 0, i2 = 1;
            int32_t v1 = row[0], v2 = row[1];
            
            if (v2 > v1) {
                int tmp_i = i1; i1 = i2; i2 = tmp_i;
                int32_t tmp_v = v1; v1 = v2; v2 = tmp_v;
            }
            
            for (int e = 2; e < experts; ++e) {
                int32_t val = row[e];
                if (val > v1 || (val == v1 && e < i1)) {
                    i2 = i1; v2 = v1;
                    i1 = e; v1 = val;
                } else if (val > v2 || (val == v2 && e < i2)) {
                    i2 = e; v2 = val;
                }
            }
            
            hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v1));
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(i1);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v2));
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(i2);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }
    return final_hash;
}
