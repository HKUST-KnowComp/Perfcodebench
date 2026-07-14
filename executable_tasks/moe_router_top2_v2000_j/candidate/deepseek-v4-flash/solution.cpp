#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline bool is_better(int32_t logit_a, int idx_a, int32_t logit_b, int idx_b) {
    return logit_a > logit_b || (logit_a == logit_b && idx_a < idx_b);
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    // Precompute top‑2 for each token once
    std::vector<uint64_t> cached(static_cast<std::size_t>(tokens) * 4);
    const int32_t* data = logits.data();
    for (int t = 0; t < tokens; ++t) {
        const int32_t* row = data + static_cast<std::size_t>(t) * experts;

        int best_idx = 0;
        int32_t best_val = row[0];
        int second_idx = 1;
        int32_t second_val = row[1];
        if (is_better(second_val, second_idx, best_val, best_idx)) {
            std::swap(best_val, second_val);
            std::swap(best_idx, second_idx);
        }

        for (int e = 2; e < experts; ++e) {
            int32_t v = row[e];
            if (is_better(v, e, best_val, best_idx)) {
                second_val = best_val; second_idx = best_idx;
                best_val = v; best_idx = e;
            } else if (is_better(v, e, second_val, second_idx)) {
                second_val = v; second_idx = e;
            }
        }

        std::size_t base = static_cast<std::size_t>(t) * 4;
        cached[base + 0] = static_cast<uint64_t>(static_cast<uint32_t>(best_val));
        cached[base + 1] = static_cast<uint64_t>(best_idx);
        cached[base + 2] = static_cast<uint64_t>(static_cast<uint32_t>(second_val));
        cached[base + 3] = static_cast<uint64_t>(second_idx);
    }

    // Hash the precomputed values 'iters' times
    uint64_t hash;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < cached.size(); ++i) {
            hash ^= cached[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
