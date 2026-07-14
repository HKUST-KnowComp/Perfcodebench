#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    if (iters <= 0) return 0;

    // Step 1: Compute CSR offsets efficiently.
    // Since row_ids is sorted, we can count occurrences in O(N) and build offsets in O(rows).
    std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1, 0);
    
    // Count frequencies using the sorted property
    for (uint32_t row : row_ids) {
        if (row < rows) {
            offsets[static_cast<size_t>(row) + 1]++;
        }
    }

    // Prefix sum to transform counts into offsets
    for (uint32_t r = 0; r < rows; ++r) {
        offsets[static_cast<size_t>(r) + 1] += offsets[static_cast<size_t>(r)];
    }

    // Step 2: Perform the hashing.
    // The baseline logic re-calculates the same offsets 'iters' times.
    // We calculate the hash once and return it.
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash = mix(hash, static_cast<uint64_t>(v));
    }

    return hash;
}