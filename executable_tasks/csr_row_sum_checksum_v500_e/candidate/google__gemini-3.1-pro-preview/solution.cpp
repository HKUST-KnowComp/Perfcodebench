#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    if (row_ptr.empty()) {
        return hash;
    }

    const uint32_t* val_ptr = values.data();
    const uint32_t* rp = row_ptr.data();
    std::size_t num_rows = row_ptr.size() - 1;

    for (std::size_t r = 0; r < num_rows; ++r) {
        std::size_t start = rp[r];
        std::size_t end = rp[r + 1];
        
        uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
        std::size_t i = start;
        
        for (; i + 3 < end; i += 4) {
            sum0 += val_ptr[i];
            sum1 += val_ptr[i+1];
            sum2 += val_ptr[i+2];
            sum3 += val_ptr[i+3];
        }
        
        uint64_t sum = sum0 + sum1 + sum2 + sum3;
        for (; i < end; ++i) {
            sum += val_ptr[i];
        }
        
        hash ^= sum;
        hash *= 1099511628211ULL;
    }

    return hash;
}
