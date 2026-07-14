#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
    inline uint64_t mix(uint64_t hash, uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
        return hash;
    }
}

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    uint64_t hash = 1469598103934665603ULL;
    const int32_t* data = logits.data();
    for (int t = 0; t < tokens; ++t) {
        const int32_t* token_logits = data + static_cast<size_t>(t) * experts;
        int best_idx = -1;
        int32_t best_val = INT32_MIN;
        int second_idx = -1;
        int32_t second_val = INT32_MIN;
        for (int e = 0; e < experts; ++e) {
            int32_t v = token_logits[e];
            if (v > best_val || (v == best_val && e < best_idx)) {
                second_idx = best_idx;
                second_val = best_val;
                best_idx = e;
                best_val = v;
            } else if (v > second_val || (v == second_val && e < second_idx)) {
                second_idx = e;
                second_val = v;
            }
        }
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
        hash = mix(hash, static_cast<uint64_t>(best_idx));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
        hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
    return hash;
}