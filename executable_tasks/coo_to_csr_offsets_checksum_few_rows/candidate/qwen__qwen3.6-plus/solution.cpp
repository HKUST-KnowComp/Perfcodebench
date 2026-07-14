#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
    inline uint64_t mix(uint64_t hash, uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
        return hash;
    }
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    uint64_t hash = 0;
    const uint32_t* ptr = row_ids.data();
    const uint32_t* end = ptr + row_ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        uint32_t current_offset = 0;
        hash = mix(hash, current_offset);
        
        const uint32_t* p = ptr;
        for (uint32_t r = 0; r < rows; ++r) {
            uint32_t count = 0;
            while (p < end && *p == r) {
                ++count;
                ++p;
            }
            current_offset += count;
            hash = mix(hash, current_offset);
        }
    }
    return hash;
}