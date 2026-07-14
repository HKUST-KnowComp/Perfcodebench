#include "interface.h"
#include <cstdint>
#include <vector>
#include <climits>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    if (iters <= 0) return 0;

    uint64_t hash = 1469598103934665603ULL;
    const int32_t* ptr = logits.data();

    for (int t = 0; t < tokens; ++t) {
        int32_t v1 = INT32_MIN, v2 = INT32_MIN;
        int i1 = experts, i2 = experts;

        for (int e = 0; e < experts; ++e) {
            int32_t val = ptr[e];
            if (val > v1 || (val == v1 && e < i1)) {
                v2 = v1; i2 = i1;
                v1 = val; i1 = e;
            } else if (val > v2 || (val == v2 && e < i2)) {
                v2 = val; i2 = e;
            }
        }
        ptr += experts;

        hash ^= static_cast<uint32_t>(v1);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint32_t>(i1);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint32_t>(v2);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint32_t>(i2);
        hash *= 1099511628211ULL;
    }

    return hash;
}