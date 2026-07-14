#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline bool is_better(int32_t val_a, int idx_a, int32_t val_b, int idx_b) {
    if (val_a != val_b) return val_a > val_b;
    return idx_a < idx_b;
}

uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int t = 0; t < tokens; ++t) {
            const std::size_t base = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
            int32_t best_val = logits[base];
            int best_idx = 0;
            int32_t second_val = (experts > 1) ? logits[base + 1] : -2147483648;
            int second_idx = (experts > 1) ? 1 : -1;
            if (is_better(second_val, second_idx, best_val, best_idx)) {
                std::swap(best_val, second_val);
                std::swap(best_idx, second_idx);
            }
            for (int e = 2; e < experts; ++e) {
                int32_t val = logits[base + e];
                if (is_better(val, e, best_val, best_idx)) {
                    second_val = best_val;
                    second_idx = best_idx;
                    best_val = val;
                    best_idx = e;
                } else if (is_better(val, e, second_val, second_idx)) {
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