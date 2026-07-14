#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> offsets;
    offsets.reserve(rows + 1);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        offsets.assign(rows + 1, 0);
        for (uint32_t r : row_ids) {
            offsets[r + 1]++;
        }
        hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i <= rows; ++i) {
            if (i > 0) offsets[i] += offsets[i - 1];
            hash ^= offsets[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}