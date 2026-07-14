#include "interface.h"
#include <cstdint>
#include <climits>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const int32_t* data = logits.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (int t = 0; t < tokens; ++t) {
            const int32_t* row = data + static_cast<size_t>(t) * experts;
            
            int32_t v1 = INT32_MIN, v2 = INT32_MIN;
            int i1 = INT32_MAX, i2 = INT32_MAX;
            
            for (int e = 0; e < experts; ++e) {
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
    }
    return hash;
}
