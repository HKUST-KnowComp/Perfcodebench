#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    std::size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
    std::vector<uint64_t> row_sums;
    row_sums.reserve(num_rows);
    
    const uint32_t* v_ptr = values.data();
    const uint32_t* rp_ptr = row_ptr.data();
    
    for (std::size_t r = 0; r < num_rows; ++r) {
        const uint32_t* start = v_ptr + rp_ptr[r];
        const uint32_t* end = v_ptr + rp_ptr[r + 1];
        uint64_t sum = 0;
        while (start != end) {
            sum += *start++;
        }
        row_sums.push_back(sum);
    }
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint64_t s : row_sums) {
            hash ^= s;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}