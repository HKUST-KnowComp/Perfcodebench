#include "interface.h"

#include <cstdint>
#include <vector>
#include <utility>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    if (iters <= 0 || tokens <= 0 || experts < 2) return 0;

    uint64_t hash = 1469598103934665603ULL;
    const int32_t* data = logits.data();
    const std::size_t stride = static_cast<std::size_t>(experts);

    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = data + static_cast<std::size_t>(t) * stride;
        
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
            if (val > v1) {
                v2 = v1; i2 = i1;
                v1 = val; i1 = e;
            } else if (val > v2) {
                v2 = val; i2 = e;
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
