#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    uint64_t hash = 0;
    const uint32_t* data = row_ids.data();
    const uint32_t* end = data + row_ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        uint32_t current_offset = 0;
        
        // Hash offsets[0] which is always 0
        h ^= current_offset;
        h *= 1099511628211ULL;

        const uint32_t* ptr = data;
        for (uint32_t r = 0; r < rows; ++r) {
            uint32_t count = 0;
            while (ptr < end && *ptr == r) {
                ++count;
                ++ptr;
            }
            current_offset += count;
            h ^= current_offset;
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}