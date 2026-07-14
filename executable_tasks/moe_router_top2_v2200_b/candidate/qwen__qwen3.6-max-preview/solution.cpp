#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
    if (iters <= 0) return 0;
    if (tokens == 0) return 1469598103934665603ULL;

    uint64_t hash = 1469598103934665603ULL;
    const int32_t* row = logits.data();

    for (int t = 0; t < tokens; ++t) {
        int32_t v0 = row[0];
        int32_t v1 = row[1];
        
        int32_t best_val, second_val;
        int best_idx, second_idx;
        
        if (v1 > v0) {
            best_val = v1; best_idx = 1;
            second_val = v0; second_idx = 0;
        } else {
            best_val = v0; best_idx = 0;
            second_val = v1; second_idx = 1;
        }
        
        for (int e = 2; e < experts; ++e) {
            int32_t v = row[e];
            if (v > best_val || (v == best_val && e < best_idx)) {
                second_val = best_val; second_idx = best_idx;
                best_val = v; best_idx = e;
            } else if (v > second_val || (v == second_val && e < second_idx)) {
                second_val = v; second_idx = e;
            }
        }
        
        hash ^= static_cast<uint32_t>(best_val);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(best_idx);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint32_t>(second_val);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(second_idx);
        hash *= 1099511628211ULL;
        
        row += experts;
    }
    
    return hash;
}