#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> offsets(rows + 1);
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        for (uint32_t i = 0; i <= rows; ++i) offsets[i] = 0;
        for (uint32_t r : row_ids) {
            offsets[r + 1]++;
        }
        for (uint32_t i = 1; i <= rows; ++i) {
            offsets[i] += offsets[i - 1];
        }
        hash = FNV_OFFSET;
        for (uint32_t v : offsets) {
            hash ^= v;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}