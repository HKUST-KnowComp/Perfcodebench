#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
    std::vector<uint32_t> counts(rows, 0);
    for (uint32_t r : row_ids) {
        ++counts[r];
    }

    uint64_t hash = 1469598103934665603ULL;
    // mix in offsets[0] = 0
    hash ^= 0;
    hash *= 1099511628211ULL;

    uint32_t prefix = 0;
    for (uint32_t r = 0; r < rows; ++r) {
        prefix += counts[r];                      // offsets[r+1]
        hash ^= prefix;
        hash *= 1099511628211ULL;
    }
    return hash;
}