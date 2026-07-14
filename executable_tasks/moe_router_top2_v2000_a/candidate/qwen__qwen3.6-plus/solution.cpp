#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    const int32_t* data = logits.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int t = 0; t < tokens; ++t) {
            int base = t * experts;
            
            int32_t v1 = data[base];
            int32_t v2 = data[base + 1];
            int i1 = 0;
            int i2 = 1;
            
            if (v2 > v1 || (v2 == v1 && i2 < i1)) {
                int32_t tv = v1; v1 = v2; v2 = tv;
                int ti = i1; i1 = i2; i2 = ti;
            }
            
            for (int e = 2; e < experts; ++e) {
                int32_t val = data[base + e];
                if (val > v1 || (val == v1 && e < i1)) {
                    v2 = v1; i2 = i1;
                    v1 = val; i1 = e;
                } else if (val > v2 || (val == v2 && e < i2)) {
                    v2 = val; i2 = e;
                }
            }
            
            hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v1)));
            hash = mix(hash, static_cast<uint64_t>(i1));
            hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v2)));
            hash = mix(hash, static_cast<uint64_t>(i2));
        }
    }
    return hash;
}
