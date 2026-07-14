#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

constexpr uint64_t SEED = 1469598103934665603ULL;
constexpr uint64_t PRIME = 1099511628211ULL;

// Computes (base^exp) mod 2^64
uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= PRIME;
    return hash;
}

struct Entry {
    uint32_t index;
    uint64_t value;
};

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    
    if (iters <= 0) return 0;
    if (indices.empty()) {
        // If no updates, hash is just SEED mixed with 0 'domain' times.
        return SEED * power(PRIME, domain);
    }

    // Step 1: Collect and sort updates to handle sparsity.
    // We only need to compute this once because indices/values don't change per iteration.
    std::vector<Entry> updates;
    updates.reserve(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        updates.push_back({indices[i], static_cast<uint64_t>(values[i])});
    }
    
    std::sort(updates.begin(), updates.end(), [](const Entry& a, const Entry& b) {
        return a.index < b.index;
    });

    // Step 2: Aggregate values for duplicate indices.
    std::vector<Entry> unique_updates;
    unique_updates.reserve(updates.size());
    if (!updates.empty()) {
        unique_updates.push_back(updates[0]);
        for (size_t i = 1; i < updates.size(); ++i) {
            if (updates[i].index == unique_updates.back().index) {
                unique_updates.back().value += updates[i].value;
            } else {
                unique_updates.push_back(updates[i]);
            }
        }
    }

    // Step 3: Compute hash skipping zeros.
    // The hash logic is: hash = (hash ^ val) * PRIME.
    // For a sequence of L zeros: hash = hash * (PRIME^L).
    uint64_t hash = SEED;
    uint32_t current_idx = 0;

    for (const auto& entry : unique_updates) {
        if (entry.index >= domain) break;
        
        // Jump over zeros before this index
        uint32_t zeros = entry.index - current_idx;
        if (zeros > 0) {
            hash *= power(PRIME, zeros);
        }
        
        // Mix the accumulated value
        hash = mix(hash, entry.value);
        current_idx = entry.index + 1;
    }

    // Jump over remaining zeros until the end of the domain
    if (current_idx < domain) {
        hash *= power(PRIME, domain - current_idx);
    }

    return hash;
}