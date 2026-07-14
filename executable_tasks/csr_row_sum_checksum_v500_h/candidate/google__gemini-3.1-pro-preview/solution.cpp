#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    std::size_t n = row_ptr.size();
    if (n < 2) {
        return hash;
    }

    const uint32_t* val_data = values.data();
    const uint32_t* row_data = row_ptr.data();

    for (std::size_t r = 0; r + 1 < n; ++r) {
        std::size_t start = row_data[r];
        std::size_t end = row_data[r + 1];
        
        uint64_t sum = 0;
        const uint32_t* current_vals = val_data + start;
        std::size_t len = end - start;
        
        for (std::size_t i = 0; i < len; ++i) {
            sum += current_vals[i];
        }
        
        hash ^= sum;
        hash *= 1099511628211ULL;
    }

    return hash;
}
