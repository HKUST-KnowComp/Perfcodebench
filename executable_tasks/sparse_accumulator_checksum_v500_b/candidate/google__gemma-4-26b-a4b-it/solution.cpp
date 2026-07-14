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

// Fast modular exponentiation for the constant multiplier
uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    
    if (indices.empty()) {
        uint64_t hash = 1469598103934665603ULL;
        uint64_t multiplier = 1099511628211ULL;
        // The original loop: for (key=0; key<domain; ++key) hash = mix(hash, 0);
        // mix(h, 0) is (h ^ 0) * multiplier = h * multiplier.
        // So we need hash * (multiplier ^ domain).
        // However, the loop runs 'iters' times. The problem implies the hash is updated per iteration.
        // Looking at the baseline: hash is reset to FNV_OFFSET inside the iter loop.
        // So we only care about the last iteration's result.
        uint64_t mult_pow = power(multiplier, domain);
        return hash * mult_pow;
    }

    // Pre-calculate powers of the multiplier to avoid repeated O(log N) calls
    // But since we only need it for gaps, we can use a simple loop or precompute if domain is small.
    // Given the constraints, we'll use a more direct approach.
    
    uint64_t final_hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t MULTIPLIER = 1099511628211ULL;

    // We only need to compute the result for the last iteration because 'hash' is overwritten
    // inside the loop: hash = 1469598103934665603ULL;
    // Wait, the baseline code: 'hash = 1469598103934665603ULL;' is INSIDE the 'iter' loop.
    // This means the 'iters' loop is actually redundant if it doesn't accumulate 'hash' across iterations.
    // It just repeats the same calculation 'iters' times. 
    // Let's check: 'hash = 1469598103934665603ULL;' is inside 'for (int iter = 0; iter < iters; ++iter)'.
    // So 'hash' is reset every time. The final value is just the result of the last iteration.

    // 1. Aggregate values
    std::vector<std::pair<uint32_t, uint64_t>> sorted_accum;
    sorted_accum.reserve(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        sorted_accum.push_back({indices[i], static_cast<uint64_t>(values[i])});
    }
    std::sort(sorted_accum.begin(), sorted_accum.end());

    std::vector<std::pair<uint32_t, uint64_t>> merged;
    if (!sorted_accum.empty()) {
        merged.push_back(sorted_accum[0]);
        for (size_t i = 1; i < sorted_accum.size(); ++i) {
            if (sorted_accum[i].first == merged.back().first) {
                merged.back().second += sorted_accum[i].second;
            } else {
                merged.push_back(sorted_accum[i]);
            }
        }
    }

    // 2. Compute hash with gaps
    uint64_t h = FNV_OFFSET;
    uint32_t current_key = 0;

    for (const auto& p : merged) {
        uint32_t key = p.first;
        uint64_t val = p.second;

        // Gap of zeros from current_key to key-1
        uint32_t gap = key - current_key;
        if (gap > 0) {
            // h = h * (MULTIPLIER ^ gap)
            // We use a loop for power to avoid complexity, but for large gaps, 
            // we need a fast way. Since we can't use math.h easily, let's use a simple power.
            // However, the baseline uses 'mix(hash, 0)'.
            // mix(h, 0) = (h ^ 0) * MULTIPLIER = h * MULTIPLIER.
            // So h_after_gap = h * (MULTIPLIER ^ gap).
            
            // To handle large gaps efficiently:
            uint64_t m_pow = 1;
            uint64_t base = MULTIPLIER;
            uint32_t exp = gap;
            while (exp > 0) {
                if (exp % 2 == 1) m_pow *= base;
                base *= base;
                exp /= 2;
            }
            h *= m_pow;
        }

        // The actual value
        h = mix(h, val);
        current_key = key + 1;
    }

    // Final gap
    if (current_key < domain) {
        uint32_t gap = domain - current_key;
        uint64_t m_pow = 1;
        uint64_t base = MULTIPLIER;
        uint32_t exp = gap;
        while (exp > 0) {
            if (exp % 2 == 1) m_pow *= base;
            base *= base;
            exp /= 2;
        }
        h *= m_pow;
    }

    return h;
}