#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    const int32_t* data = logits.data();
    const std::size_t stride = static_cast<std::size_t>(experts);

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        const int32_t* p = data;
        for (int t = 0; t < tokens; ++t) {
            int32_t v1 = p[0];
            int i1 = 0;
            int32_t v2 = p[1];
            int i2 = 1;

            if (v2 > v1 || (v2 == v1 && i2 < i1)) {
                int32_t tmp_v = v1; v1 = v2; v2 = tmp_v;
                int tmp_i = i1; i1 = i2; i2 = tmp_i;
            }

            for (int e = 2; e < experts; ++e) {
                int32_t v = p[e];
                if (v > v1 || (v == v1 && e < i1)) {
                    v2 = v1; i2 = i1;
                    v1 = v; i1 = e;
                } else if (v > v2 || (v == v2 && e < i2)) {
                    v2 = v; i2 = e;
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

            p += stride;
        }
    }
    return hash;
}