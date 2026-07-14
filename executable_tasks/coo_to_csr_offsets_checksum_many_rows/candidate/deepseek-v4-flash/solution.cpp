#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> counts(rows, 0);
    for (uint32_t row : row_ids) {
        if (row < rows) ++counts[row];
    }
    std::vector<uint32_t> offsets(rows + 1, 0);
    uint32_t sum = 0;
    for (uint32_t r = 0; r < rows; ++r) {
        sum += counts[r];
        offsets[r + 1] = sum;
    }
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash ^= v;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
