#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0 || row_ptr.size() < 2) return 0;
    
    std::size_t num_rows = row_ptr.size() - 1;
    std::vector<uint64_t> row_sums(num_rows);
    
    const uint32_t* rp = row_ptr.data();
    const uint32_t* val = values.data();
    
    for (std::size_t r = 0; r < num_rows; ++r) {
        uint64_t sum = 0;
        uint32_t start = rp[r];
        uint32_t end = rp[r + 1];
        for (uint32_t i = start; i < end; ++i) {
            sum += val[i];
        }
        row_sums[r] = sum;
    }
    
    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t* sums = row_sums.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (std::size_t r = 0; r < num_rows; ++r) {
            hash ^= sums[r];
            hash *= FNV_PRIME;
        }
    }
    return hash;
}