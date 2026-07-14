#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    const uint32_t* val_ptr = values.data();
    const uint32_t* rp_ptr = row_ptr.data();
    const std::size_t n = row_ptr.size();
    
    for (std::size_t r = 0; r + 1 < n; ++r) {
        std::size_t start = rp_ptr[r];
        std::size_t end = rp_ptr[r + 1];
        uint64_t sum = 0;
        for (std::size_t i = start; i < end; ++i) {
            sum += val_ptr[i];
        }
        hash ^= sum;
        hash *= 1099511628211ULL;
    }
    return hash;
}