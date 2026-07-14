#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    const size_t nnz = row_ids.size();
    const uint32_t* data = row_ids.data();
    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        // offsets[0] is always 0
        hash ^= 0;
        hash *= FNV_PRIME;

        size_t idx = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            while (idx < nnz && data[idx] == r) {
                ++idx;
            }
            hash ^= static_cast<uint32_t>(idx);
            hash *= FNV_PRIME;
        }
    }
    return hash;
}
