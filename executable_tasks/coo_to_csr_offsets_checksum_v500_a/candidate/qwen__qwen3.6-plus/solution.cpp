#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> offsets(rows + 1);
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(offsets.begin(), offsets.end(), 0);
        for (uint32_t r : row_ids) {
            offsets[r + 1]++;
        }
        
        hash = 1469598103934665603ULL;
        // offsets[0] is always 0, so hash ^= 0 is a no-op, just multiply by prime
        hash *= 1099511628211ULL;
        
        for (uint32_t i = 1; i <= rows; ++i) {
            offsets[i] += offsets[i - 1];
            hash ^= offsets[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}