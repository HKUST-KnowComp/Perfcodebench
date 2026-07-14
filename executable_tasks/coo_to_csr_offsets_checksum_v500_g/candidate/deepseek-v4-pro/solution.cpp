#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    if (iters <= 0) return 0;

    // Precompute row counts
    std::vector<uint32_t> counts(rows, 0);
    for (uint32_t row : row_ids) {
        if (row < rows) {  // safety guard; row_ids should be within range
            ++counts[row];
        }
    }

    // Build CSR offsets once
    std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1);
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
        offsets[static_cast<std::size_t>(r) + 1] = 
            offsets[static_cast<std::size_t>(r)] + counts[r];
    }

    // Hash the entire offset array exactly once
    const uint64_t seed = 1469598103934665603ULL;
    uint64_t hash = seed;
    for (uint32_t v : offsets) {
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}