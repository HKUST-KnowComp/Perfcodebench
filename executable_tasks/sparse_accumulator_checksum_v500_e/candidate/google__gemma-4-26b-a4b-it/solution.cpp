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

// Precompute the effect of mixing 0 into the hash multiple times.
// Since the original loop does: hash = mix(hash, 0) for every key in domain.
// We can precompute this or observe the pattern.
// However, for a general solution that is fast, we can precompute a table or 
// just realize that if we skip zeros, we need to know what the hash becomes 
// after N calls to mix(h, 0).

struct Entry {
    uint32_t idx;
    uint64_t val;
};

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
                hash = mix(hash, 0ULL);
            }
        }
        return hash;
    }

    // Precompute the transformation for mixing 0
    // f(h) = mix(h, 0) = (h ^ 0) * 1099511628211ULL
    // This is a linear transformation in the ring of 2^64 integers: h -> h * C
    const uint64_t C = 1099511628211ULL;

    uint64_t final_hash = 1469598103934665603ULL;

    // We need to perform the accumulation and hashing 'iters' times.
    // Note: The original code clears the accumulator every iteration, 
    // but the input indices/values don't change. 
    // So the accumulator content is identical every iteration.
    
    // 1. Accumulate values
    std::vector<Entry> sorted_entries;
    sorted_entries.reserve(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        sorted_entries.push_back({indices[i], static_cast<uint64_t>(values[i])});
    }
    std::sort(sorted_entries.begin(), sorted_entries.end(), [](const Entry& a, const Entry& b) {
        return a.idx < b.idx;
    });

    std::vector<Entry> compressed;
    if (!sorted_entries.empty()) {
        compressed.push_back(sorted_entries[0]);
        for (size_t i = 1; i < sorted_entries.size(); ++i) {
            if (sorted_entries[i].idx == compressed.back().idx) {
                compressed.back().val += sorted_entries[i].val;
            } else {
                compressed.push_back(sorted_entries[i]);
            }
        }
    }

    // 2. Compute the hash for one iteration
    // The hash loop: for k=0 to domain-1: hash = mix(hash, accum[k])
    // If k is not in compressed, accum[k] = 0.
    
    auto compute_single_iter = [&](uint64_t h) {
        uint32_t current_idx = 0;
        for (const auto& entry : compressed) {
            // Gap of zeros from current_idx to entry.idx - 1
            uint32_t gap = entry.idx - current_idx;
            if (gap > 0) {
                // Applying mix(h, 0) 'gap' times is h * (C^gap)
                // We can use binary exponentiation for C^gap
                uint64_t multiplier = 1;
                uint64_t base = C;
                uint32_t exp = gap;
                while (exp > 0) {
                    if (exp % 2 == 1) multiplier *= base;
                    base *= base;
                    exp /= 2;
                }
                h *= multiplier;
            }
            h = mix(h, entry.val);
            current_idx = entry.idx + 1;
        }
        // Final gap
        if (current_idx < domain) {
            uint32_t gap = domain - current_idx;
            uint64_t multiplier = 1;
            uint64_t base = C;
            uint32_t exp = gap;
            while (exp > 0) {
                if (exp % 2 == 1) multiplier *= base;
                base *= base;
                exp /= 2;
            }
            h *= multiplier;
        }
        return h;
    };

    // Since the accumulator is cleared and rebuilt with the same data every iteration,
    // the hash is updated as: hash_next = compute_single_iter(hash_prev)
    // Wait, the original code says: hash = 1469598103934665603ULL; inside the loop.
    // This means the hash is NOT cumulative across iterations. 
    // It is reset to the FNV offset basis every iteration.
    // Therefore, we only need to compute it once and return it.
    
    // Re-reading original code:
    // for (int iter = 0; iter < iters; ++iter) {
    //    accum.clear(); ... 
    //    hash = 1469598103934665603ULL;
    //    for (uint32_t key = 0; key < domain; ++key) { ... hash = mix(hash, ...); }
    // }
    // return hash;
    // Yes, the hash is reset every iteration. The result is just the result of the last iteration.

    uint64_t h = 1469598103934665603ULL;
    uint32_t current_idx = 0;
    for (const auto& entry : compressed) {
        uint32_t gap = entry.idx - current_idx;
        if (gap > 0) {
            uint64_t multiplier = 1;
            uint64_t base = C;
            uint32_t exp = gap;
            while (exp > 0) {
                if (exp % 2 == 1) multiplier *= base;
                base *= base;
                exp /= 2;
            }
            h *= multiplier;
        }
        h = mix(h, entry.val);
        current_idx = entry.idx + 1;
    }
    if (current_idx < domain) {
        uint32_t gap = domain - current_idx;
        uint64_t multiplier = 1;
        uint64_t base = C;
        uint32_t exp = gap;
        while (exp > 0) {
            if (exp % 2 == 1) multiplier *= base;
            base *= base;
            exp /= 2;
        }
        h *= multiplier;
    }

    return h;
}