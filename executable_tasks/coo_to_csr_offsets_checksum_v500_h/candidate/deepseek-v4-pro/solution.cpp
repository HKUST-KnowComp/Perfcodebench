#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    (void)iters; // hash resets each iteration in original, so final result is independent of iterations
    std::vector<uint32_t> counts(rows, 0);
    for (uint32_t row : row_ids) {
        ++counts[row];
    }
    std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1);
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
        offsets[static_cast<std::size_t>(r) + 1] = offsets[r] + counts[r];
    }
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}