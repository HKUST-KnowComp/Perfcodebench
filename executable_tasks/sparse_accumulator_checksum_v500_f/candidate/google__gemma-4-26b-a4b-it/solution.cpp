#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Precompute the transformation for a single zero: h = mix(h, 0)
// This is a linear transformation in the sense of h_new = h * multiplier + offset
// where offset is 0 because mix(h, 0) = (h ^ 0) * 1099511628211
// So mix(h, 0) = h * 1099511628211
// Applying it K times: h_k = h * (1099511628211^K)

struct Pair {
    uint32_t idx;
    uint32_t val;
};

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    if (domain == 0) return 0;

    // Precompute powers of the multiplier to jump over zero gaps
    const uint64_t multiplier = 1099511628211ULL;
    std::vector<uint64_t> pow_mult(64);
    pow_mult[0] = 1;
    for (int i = 1; i < 64; ++i) pow_mult[i] = pow_mult[i - 1] * multiplier;

    auto get_jump = [&](uint32_t k) -> uint64_t {
        uint64_t res = 1;
        for (int i = 0; i < 32; ++i) {
            if ((k >> i) & 1) res *= pow_mult[i];
        }
        // Note: The above is a bit slow, let's use a simpler binary exponentiation
        uint64_t base = multiplier;
        uint64_t ret = 1;
        while (k > 0) {
            if (k & 1) ret *= base;
            base *= base;
            k >>= 1;
        }
        return ret;
    };

    // Since we need to call get_jump many times, let's use a faster way
    // or just precompute a small table for the jump.
    // Actually, the number of non-zero elements is small. 
    // We can use a simple binary exponentiation inside the loop.

    uint64_t final_hash = 0;
    std::vector<Pair> pairs(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        pairs[i] = {indices[i], values[i]};
    }

    for (int iter = 0; iter < iters; ++iter) {
        std::sort(pairs.begin(), pairs.end(), [](const Pair& a, const Pair& b) {
            return a.idx < b.idx;
        });

        // Aggregate duplicates
        std::vector<std::pair<uint32_t, uint64_t>> accum;
        if (!pairs.empty()) {
            uint32_t curr_idx = pairs[0].idx;
            uint64_t curr_val = pairs[0].val;
            for (size_t i = 1; i < pairs.size(); ++i) {
                if (pairs[i].idx == curr_idx) {
                    curr_val += pairs[i].val;
                } else {
                    accum.push_back({curr_idx, curr_val});
                    curr_idx = pairs[i].idx;
                    curr_val = pairs[i].val;
                }
            }
            accum.push_back({curr_idx, curr_val});
        }

        uint64_t hash = 1469598103934665603ULL;
        uint32_t current_pos = 0;

        for (const auto& p : accum) {
            // 1. Handle zeros before this index
            if (p.first > current_pos) {
                uint32_t gap = p.first - current_pos;
                // hash = hash * (multiplier ^ gap)
                uint64_t m = multiplier;
                uint64_t res = 1;
                uint32_t k = gap;
                while (k > 0) {
                    if (k & 1) res *= m;
                    m *= m;
                    k >>= 1;
                }
                hash *= res;
            }
            // 2. Handle the non-zero value
            hash = mix(hash, p.second);
            current_pos = p.first + 1;
        }

        // 3. Handle trailing zeros
        if (current_pos < domain) {
            uint32_t gap = domain - current_pos;
            uint64_t m = multiplier;
            uint64_t res = 1;
            uint32_t k = gap;
            while (k > 0) {
                if (k & 1) res *= m;
                m *= m;
                k >>= 1;
            }
            hash *= res;
        }
        final_hash = hash;
    }

    return final_hash;
}
