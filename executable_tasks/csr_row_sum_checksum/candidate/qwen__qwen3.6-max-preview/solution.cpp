#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    const size_t n_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
    std::vector<uint64_t> row_sums(n_rows);
    
    const uint32_t* rp = row_ptr.data();
    const uint32_t* val = values.data();
    
    for (size_t r = 0; r < n_rows; ++r) {
        uint64_t sum = 0;
        size_t start = rp[r];
        size_t end = rp[r + 1];
        for (size_t i = start; i < end; ++i) {
            sum += val[i];
        }
        row_sums[r] = sum;
    }
    
    uint64_t hash = 0;
    const uint64_t* sums = row_sums.data();
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t r = 0; r < n_rows; ++r) {
            hash ^= sums[r];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
