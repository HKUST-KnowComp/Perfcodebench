#include "interface.h"

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    uint64_t hash = 0;
    const size_t n = row_ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        // Hash offsets[0] which is always 0
        hash *= 1099511628211ULL;
        
        uint32_t current_offset = 0;
        size_t idx = 0;
        
        for (uint32_t r = 0; r < rows; ++r) {
            while (idx < n && row_ids[idx] == r) {
                ++current_offset;
                ++idx;
            }
            hash ^= current_offset;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}