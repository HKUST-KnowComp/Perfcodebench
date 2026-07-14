#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    const uint32_t* __restrict__ rp = row_ptr.data();
    const uint32_t* __restrict__ vals = values.data();
    const std::size_t num_rows = row_ptr.size() - 1;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        for (std::size_t r = 0; r < num_rows; ++r) {
            const std::size_t start = rp[r];
            const std::size_t end = rp[r + 1];
            const uint32_t* row_vals = vals + start;
            std::size_t len = end - start;
            
            uint64_t sum = 0;
            std::size_t i = 0;
            
            // Unroll by 8 for better throughput
            for (; i + 8 <= len; i += 8) {
                sum += row_vals[i];
                sum += row_vals[i + 1];
                sum += row_vals[i + 2];
                sum += row_vals[i + 3];
                sum += row_vals[i + 4];
                sum += row_vals[i + 5];
                sum += row_vals[i + 6];
                sum += row_vals[i + 7];
            }
            
            // Handle remainder
            for (; i < len; ++i) {
                sum += row_vals[i];
            }
            
            // FNV-1a style mix
            hash ^= sum;
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}