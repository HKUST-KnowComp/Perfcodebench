#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    const uint32_t* rp = row_ptr.data();
    const uint32_t* vals = values.data();
    const std::size_t num_rows = row_ptr.size() - 1;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        for (std::size_t r = 0; r < num_rows; ++r) {
            const uint32_t start = rp[r];
            const uint32_t end = rp[r + 1];
            const uint32_t* row_start = vals + start;
            const uint32_t len = end - start;
            
            uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
            uint32_t i = 0;
            
            // Unroll by 4
            const uint32_t len4 = len & ~3u;
            for (; i < len4; i += 4) {
                sum0 += row_start[i];
                sum1 += row_start[i + 1];
                sum2 += row_start[i + 2];
                sum3 += row_start[i + 3];
            }
            
            // Handle remainder
            for (; i < len; ++i) {
                sum0 += row_start[i];
            }
            
            uint64_t sum = sum0 + sum1 + sum2 + sum3;
            
            // FNV-1a style mix
            hash ^= sum;
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}