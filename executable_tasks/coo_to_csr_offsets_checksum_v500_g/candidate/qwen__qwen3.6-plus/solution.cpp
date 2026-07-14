#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    (void)iters;
    uint64_t hash = 1469598103934665603ULL;
    uint32_t current_offset = 0;

    // Hash offsets[0] which is always 0
    hash ^= 0;
    hash *= 1099511628211ULL;

    uint32_t idx = 0;
    for (uint32_t r : row_ids) {
        while (idx < r) {
            hash ^= current_offset;
            hash *= 1099511628211ULL;
            ++idx;
        }
        ++current_offset;
    }
    while (idx < rows) {
        hash ^= current_offset;
        hash *= 1099511628211ULL;
        ++idx;
    }

    return hash;
}