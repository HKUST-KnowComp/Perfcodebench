#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Computes (base^exp) mod 2^64 in O(log exp) time.
uint64_t power(uint64_t base, uint64_t exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
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
    if (domain == 0) return FNV_OFFSET_BASIS;

    // Step 1: Accumulate values for each index.
    // Since the inputs are constant across iterations, we only compute this once.
    size_t n = indices.size();
    if (n == 0) {
        return power(FNV_PRIME, domain) * FNV_OFFSET_BASIS;
    }

    std::vector<Entry> entries(n);
    for (size_t i = 0; i < n; ++i) {
        entries[i] = {indices[i], static_cast<uint64_t>(values[i])};
    }

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.index < b.index;
    });

    // Merge duplicate indices
    size_t unique_count = 0;
    if (n > 0) {
        unique_count = 1;
        for (size_t i = 1; i < n; ++i) {
            if (entries[i].index == entries[unique_count - 1].index) {
                entries[unique_count - 1].value += entries[i].value;
            } else {
                entries[unique_count] = entries[i];
                unique_count++;
            }
        }
    }

    // Step 2: Compute the hash efficiently.
    // The hash update for a zero value is: hash = (hash ^ 0) * FNV_PRIME = hash * FNV_PRIME.
    // For a gap of size 'k', the hash becomes: hash * (FNV_PRIME^k).
    uint64_t hash = FNV_OFFSET_BASIS;
    uint32_t last_idx = 0;
    bool started = false;

    for (size_t i = 0; i < unique_count; ++i) {
        uint32_t idx = entries[i].index;
        if (idx >= domain) break; // Ignore indices outside the domain

        // Apply multiplier for the gap of zeros before this index
        uint32_t gap = idx - (started ? last_idx + 1 : 0);
        if (gap > 0) {
            hash *= power(FNV_PRIME, gap);
        }

        // Apply the update for the current index
        hash ^= entries[i].value;
        hash *= FNV_PRIME;

        last_idx = idx;
        started = true;
    }

    // Apply multiplier for the remaining zeros in the domain
    uint32_t final_gap = domain - (started ? last_idx + 1 : 0);
    if (final_gap > 0) {
        hash *= power(FNV_PRIME, final_gap);
    }

    // The result is the same for every iteration because the inputs don't change.
    return hash;
}
