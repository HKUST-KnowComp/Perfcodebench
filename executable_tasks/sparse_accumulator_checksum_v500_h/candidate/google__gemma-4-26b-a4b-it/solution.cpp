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

struct Entry {
    uint32_t idx;
    uint32_t val;
};

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    if (indices.empty()) {
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < domain; ++i) {
            hash = mix(hash, 0ULL);
        }
        return hash;
    }

    // Pre-calculate the effect of a single zero-value mix to handle gaps efficiently.
    // However, since the mix function is not easily reversible or jumpable via simple power,
    // and the domain can be large, we must observe that the hash is a sequence of operations.
    // If domain is small, we can just loop. If domain is large, we must optimize.
    
    // Let's refine the approach: The hash is a chain: h_{i} = mix(h_{i-1}, v_i).
    // Since we need to return the exact checksum, we must simulate the loop.
    // The bottleneck is the loop over 'domain'.
    
    // Optimization: Pre-calculate the 'zero-mix' chain if possible, but the mix function 
    // depends on the previous hash. We can't skip. 
    // Wait, the baseline iterates 0 to domain-1. 
    // If domain is 10^9, O(domain) is impossible. 
    // But the task is to optimize the *provided* baseline. 
    // The baseline is O(iters * domain). 
    
    // Let's check if we can optimize the zero-value accumulation.
    // The hash is: h_i = (h_{i-1} ^ v_i) * 1099511628211
    // This is a linear congruential generator style step. 
    // For v_i = 0: h_i = h_{i-1} * 1099511628211
    // This is a geometric progression in the ring of integers mod 2^64.
    // h_{i+k} = h_i * (1099511628211^k) mod 2^64.

    uint64_t multiplier = 1099511628211ULL;
    
    // Precompute powers of multiplier for fast jumping
    // We'll use a simple jump table or binary exponentiation logic.
    auto power = [](uint64_t base, uint64_t exp) {
        uint64_t res = 1;
        while (exp > 0) {
            if (exp % 2 == 1) res *= base;
            base *= base;
            exp /= 2;
        }
        return res;
    };

    // Pre-calculate powers for binary lifting to jump over zero gaps
    const int MAX_LOG = 32;
    std::vector<uint64_t> pows(MAX_LOG);
    pows[0] = multiplier;
    for (int i = 1; i < MAX_LOG; ++i) {
        pows[i] = pows[i - 1] * pows[i - 1];
    }

    auto jump_zeros = [&](uint64_t h, uint32_t count) {
        for (int i = 0; i < MAX_LOG; ++i) {
            if ((count >> i) & 1) {
                h *= pows[i];
            }
        }
        return h;
    };

    // Prepare entries for sorting
    std::vector<Entry> entries(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        entries[i] = {indices[i], values[i]};
    }

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // 1. Accumulate values
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
            return a.idx < b.idx;
        });

        std::vector<std::pair<uint32_t, uint64_t>> accum;
        if (!entries.empty()) {
            uint32_t curr_idx = entries[0].idx;
            uint64_t curr_val = entries[0].val;
            for (size_t i = 1; i < entries.size(); ++i) {
                if (entries[i].idx == curr_idx) {
                    curr_val += entries[i].val;
                } else {
                    accum.push_back({curr_idx, curr_val});
                    curr_idx = entries[i].idx;
                    curr_val = entries[i].val;
                }
            }
            accum.push_back({curr_idx, curr_val});
        }

        // 2. Compute hash with jumps
        uint64_t h = 1469598103934665603ULL;
        uint32_t current_pos = 0;

        for (const auto& p : accum) {
            uint32_t target_idx = p.first;
            uint64_t val = p.second;

            // Gap of zeros before this index
            if (target_idx > current_pos) {
                h = jump_zeros(h, target_idx - current_pos);
            }
            
            // The actual value at target_idx
            h ^= val;
            h *= multiplier;
            
            current_pos = target_idx + 1;
        }

        // Remaining zeros after the last entry
        if (current_pos < domain) {
            h = jump_zeros(h, domain - current_pos);
        }
        
        final_hash = h;
    }

    return final_hash;
}
