#include "interface.h"
#include <cstdint>
#include <vector>
#include <climits>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}
} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    std::vector<int32_t> t1_val(tokens), t2_val(tokens);
    std::vector<int> t1_idx(tokens), t2_idx(tokens);

    const int32_t* data = logits.data();
    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
        int32_t v1 = INT32_MIN, v2 = INT32_MIN;
        int i1 = experts, i2 = experts;

        for (int e = 0; e < experts; ++e) {
            int32_t v = row[e];
            if (v > v1 || (v == v1 && e < i1)) {
                v2 = v1; i2 = i1;
                v1 = v; i1 = e;
            } else if (v > v2 || (v == v2 && e < i2)) {
                v2 = v; i2 = e;
            }
        }
        t1_val[t] = v1;
        t1_idx[t] = i1;
        t2_val[t] = v2;
        t2_idx[t] = i2;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int t = 0; t < tokens; ++t) {
            hash = mix(hash, static_cast<uint32_t>(t1_val[t]));
            hash = mix(hash, static_cast<uint64_t>(t1_idx[t]));
            hash = mix(hash, static_cast<uint32_t>(t2_val[t]));
            hash = mix(hash, static_cast<uint64_t>(t2_idx[t]));
        }
    }
    return hash;
}