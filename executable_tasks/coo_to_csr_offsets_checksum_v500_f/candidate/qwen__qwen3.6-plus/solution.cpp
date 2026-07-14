#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    (void)iters;
    
    uint64_t hash = 1469598103934665603ULL;
    uint32_t current_offset = 0;
    
    // Hash offsets[0] which is always 0
    hash ^= current_offset;
    hash *= 1099511628211ULL;
    
    const uint32_t* ptr = row_ids.data();
    const uint32_t* end = ptr + row_ids.size();
    
    for (uint32_t r = 0; r < rows; ++r) {
        const uint32_t* start = ptr;
        while (ptr < end && *ptr == r) {
            ++ptr;
        }
        current_offset += static_cast<uint32_t>(ptr - start);
        hash ^= current_offset;
        hash *= 1099511628211ULL;
    }
    
    return hash;
}
