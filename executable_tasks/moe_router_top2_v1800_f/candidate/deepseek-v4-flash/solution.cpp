#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline bool better_than(int32_t val_a, int idx_a, int32_t val_b, int idx_b) {
    return val_a > val_b || (val_a == val_b && idx_a < idx_b);
}

uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    uint64_t hash = 0;
    const int32_t* data = logits.data();
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int t = 0; t < tokens; ++t) {
            const std::size_t offset = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
            const int32_t* row = data + offset;
            int best_idx = 0;
            int second_idx = 1;
            int32_t best_val = row[0];
            int32_t second_val = row[1];
            // Ensure (best_val, best_idx) is actually better than (second_val, second_idx)
            if (better_than(second_val, second_idx, best_val, best_idx)) {
                int32_t tmp_val = best_val;
                int tmp_idx = best_idx;
                best_val = second_val;
                best_idx = second_idx;
                second_val = tmp_val;
                second_idx = tmp_idx;
            }
            // Scan remaining experts
            for (int e = 2; e < experts; ++e) {
                int32_t val = row[e];
                if (better_than(val, e, best_val, best_idx)) {
                    second_val = best_val;
                    second_idx = best_idx;
                    best_val = val;
                    best_idx = e;
                } else if (better_than(val, e, second_val, second_idx)) {
                    second_val = val;
                    second_idx = e;
                }
            }
            hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
            hash = mix(hash, static_cast<uint64_t>(best_idx));
            hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
            hash = mix(hash, static_cast<uint64_t>(second_idx));
        }
    }
    return hash;
}
