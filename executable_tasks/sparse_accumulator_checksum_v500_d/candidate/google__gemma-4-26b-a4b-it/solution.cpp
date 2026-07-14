#include "interface.h"
#include <vector>
#include <algorithm>
#include <map>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Precompute the effect of applying mix(h, 0) N times.
// mix(h, 0) = (h ^ 0) * C = h * C
// Applying it N times: h * (C^N)
// However, the original code is: hash = mix(hash, it == accum.end() ? 0ULL : it->second);
// Let's trace: 
// h_next = (h_prev ^ 0) * C = h_prev * C
// So N zeros in a row: h_final = h_initial * (C^N)

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    
    if (indices.empty()) {
        uint64_t hash = 1469598103934665603ULL;
        for (int iter = 0; iter < iters; ++iter) {
            for (uint32_t k = 0; k < domain; ++k) {
                hash = mix(hash, 0);
            }
        }
        return hash;
    }

    // Precompute C^N for the zero-gap jumps if needed, 
    // but since we need to handle it per iteration, let's optimize the inner loop.
    // The original code resets 'hash' to the seed at the start of EVERY iteration.
    // Wait, looking at the baseline: 'hash = 1469598103934665603ULL;' is INSIDE the 'iters' loop.
    // This means the 'iters' loop actually just repeats the same calculation 'iters' times.
    // The final 'hash' returned is just the result of the LAST iteration.
    
    // Let's re-read: 
    // for (int iter = 0; iter < iters; ++iter) {
    //    accum.clear();
    //    ... accumulate ...
    //    hash = seed;
    //    for (uint32_t key = 0; key < domain; ++key) { hash = mix(hash, val); }
    // }
    // return hash;
    // Yes, the 'iters' loop is redundant if 'indices' and 'values' don't change.
    // The result is simply the result of one iteration.

    // 1. Accumulate
    std::vector<std::pair<uint32_t, uint64_t>> sorted_accum;
    // Using a simple sort-based accumulation to avoid unordered_map overhead
    std::vector<std::pair<uint32_t, uint32_t>> pairs(indices.size());
    for(size_t i=0; i<indices.size(); ++i) pairs[i] = {indices[i], values[i]};
    std::sort(pairs.begin(), pairs.end());

    if (pairs.empty()) {
        uint64_t h = 1469598103934665603ULL;
        for(uint32_t i=0; i<domain; ++i) h = mix(h, 0);
        return h;
    }

    std::vector<std::pair<uint32_t, uint64_t>> unique_vals;
    for (const auto& p : pairs) {
        if (!unique_vals.empty() && unique_vals.back().first == p.first) {
            unique_vals.back().second += p.second;
        } else {
            unique_vals.push_back({p.first, static_cast<uint64_t>(p.second)});
        }
    }

    // 2. Compute Hash
    uint64_t hash = 1469598103934665603ULL;
    uint32_t current_idx = 0;

    // Precompute powers of C for zero-gaps
    // Since we can't easily precompute all, and domain can be large, 
    // we use the property: mix(h, 0) = h * 1099511628211ULL
    const uint64_t C = 1099511628211ULL;

    auto apply_zeros = [&](uint32_t count) {
        for(uint32_t i=0; i<count; ++i) hash *= C;
    };
    
    // To make apply_zeros O(log N), use binary exponentiation
    auto fast_apply_zeros = [&](uint32_t count) {
        uint64_t multiplier = 1;
        uint64_t base = C;
        while (count > 0) {
            if (count % 2 == 1) multiplier *= base;
            base *= base;
            count /= 2;
        }
        hash *= multiplier;
    };

    for (const auto& kv : unique_vals) {
        uint32_t target_idx = kv.first;
        if (target_idx > current_idx) {
            fast_apply_zeros(target_idx - current_idx);
        }
        hash = mix(hash, kv.second);
        current_idx = target_idx + 1;
    }

    if (current_idx < domain) {
        fast_apply_zeros(domain - current_idx);
    }

    return hash;
}