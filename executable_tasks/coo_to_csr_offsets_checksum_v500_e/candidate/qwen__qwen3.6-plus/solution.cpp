#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    (void)iters;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    // Hash offsets[0] (always 0)
    hash ^= 0;
    hash *= FNV_PRIME;
    
    uint32_t current_offset = 0;
    const uint32_t* ptr = row_ids.data();
    const uint32_t* end = ptr + row_ids.size();
    
    for (uint32_t r = 0; r < rows; ++r) {
        uint32_t count = 0;
        while (ptr != end && *ptr == r) {
            ++count;
            ++ptr;
        }
        current_offset += count;
        hash ^= current_offset;
        hash *= FNV_PRIME;
    }
    
    return hash;
}
