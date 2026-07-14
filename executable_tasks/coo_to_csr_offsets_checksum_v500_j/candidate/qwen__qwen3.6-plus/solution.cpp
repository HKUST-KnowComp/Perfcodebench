#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> offsets(rows + 1);
    uint64_t hash = 0;
    const size_t nnz = row_ids.size();
    const uint32_t* rid_ptr = row_ids.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        offsets[0] = 0;
        size_t idx = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            while (idx < nnz && rid_ptr[idx] == r) {
                ++idx;
            }
            offsets[r + 1] = static_cast<uint32_t>(idx);
        }
        
        hash = 1469598103934665603ULL;
        const uint32_t* off_ptr = offsets.data();
        for (size_t i = 0, n = offsets.size(); i < n; ++i) {
            hash ^= off_ptr[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}