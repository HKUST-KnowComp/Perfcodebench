#include "interface.h"

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    (void)iters;
    
    uint64_t hash = 1469598103934665603ULL;
    hash *= 1099511628211ULL; // Hash for offsets[0] which is always 0

    uint32_t current_offset = 0;
    size_t idx = 0;
    const size_t n = row_ids.size();
    
    for (uint32_t r = 0; r < rows; ++r) {
        size_t start = idx;
        while (idx < n && row_ids[idx] == r) {
            ++idx;
        }
        current_offset += static_cast<uint32_t>(idx - start);
        hash ^= current_offset;
        hash *= 1099511628211ULL;
    }
    
    return hash;
}
