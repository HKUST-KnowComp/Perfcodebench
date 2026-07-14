#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    struct Top2 {
        int32_t v1; int i1;
        int32_t v2; int i2;
    };
    std::vector<Top2> cache(tokens);
    const int32_t* l = logits.data();
    const int stride = experts;

    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = l + t * stride;
        Top2 res;
        // Initialize with first two elements, respecting tie-breaking (lower index wins)
        if (row[0] >= row[1]) {
            res.v1 = row[0]; res.i1 = 0;
            res.v2 = row[1]; res.i2 = 1;
        } else {
            res.v1 = row[1]; res.i1 = 1;
            res.v2 = row[0]; res.i2 = 0;
        }

        for (int e = 2; e < experts; ++e) {
            int32_t val = row[e];
            if (val > res.v1 || (val == res.v1 && e < res.i1)) {
                res.v2 = res.v1; res.i2 = res.i1;
                res.v1 = val; res.i1 = e;
            } else if (val > res.v2 || (val == res.v2 && e < res.i2)) {
                res.v2 = val; res.i2 = e;
            }
        }
        cache[t] = res;
    }

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (int t = 0; t < tokens; ++t) {
            const auto& c = cache[t];
            hash ^= static_cast<uint64_t>(static_cast<uint32_t>(c.v1));
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(c.i1);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(static_cast<uint32_t>(c.v2));
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(c.i2);
            hash *= FNV_PRIME;
        }
    }
    return hash;
}