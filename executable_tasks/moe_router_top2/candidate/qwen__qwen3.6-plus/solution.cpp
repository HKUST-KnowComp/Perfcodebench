#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    if (iters <= 0 || tokens <= 0 || experts < 2) return 0;

    const int32_t* data = logits.data();
    uint64_t hash = 1469598103934665603ULL;

    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = data + static_cast<size_t>(t) * experts;
        
        int32_t v1 = row[0];
        int i1 = 0;
        int32_t v2 = row[1];
        int i2 = 1;
        
        if (v2 > v1 || (v2 == v1 && i2 < i1)) {
            int32_t tv = v1; int ti = i1;
            v1 = v2; i1 = i2;
            v2 = tv; i2 = ti;
        }
        
        for (int e = 2; e < experts; ++e) {
            int32_t v = row[e];
            if (v > v1 || (v == v1 && e < i1)) {
                v2 = v1; i2 = i1;
                v1 = v; i1 = e;
            } else if (v > v2 || (v == v2 && e < i2)) {
                v2 = v; i2 = e;
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
    
    return hash;
}
