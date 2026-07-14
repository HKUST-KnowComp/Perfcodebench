#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (row_ptr.size() <= 1) {
        return 1469598103934665603ULL;
    }

    const uint32_t* rp = row_ptr.data();
    const uint32_t* vp = values.data();
    const std::size_t num_rows = row_ptr.size() - 1;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t r = 0; r < num_rows; ++r) {
            const uint32_t* v_start = vp + rp[r];
            const uint32_t* v_end = vp + rp[r + 1];
            uint64_t sum = 0;
            while (v_start != v_end) {
                sum += *v_start++;
            }
            hash ^= sum;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}