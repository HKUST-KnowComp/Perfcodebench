#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    uint64_t hash = 0;
    const int32_t* data = logits.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const int32_t* row = data;
        for (int t = 0; t < tokens; ++t) {
            int32_t v1 = row[0];
            int32_t v2 = row[1];
            int i1 = 0;
            int i2 = 1;
            
            if (v2 > v1) {
                v1 = row[1]; i1 = 1;
                v2 = row[0]; i2 = 0;
            }
            
            for (int e = 2; e < experts; ++e) {
                int32_t v = row[e];
                if (v > v2) {
                    if (v > v1) {
                        v2 = v1; i2 = i1;
                        v1 = v; i1 = e;
                    } else {
                        v2 = v; i2 = e;
                    }
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
            
            row += experts;
        }
    }
    return hash;
}
