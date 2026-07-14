#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    const std::size_t total_size = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
    const std::size_t cols_sz = static_cast<std::size_t>(cols);
    
    std::vector<uint32_t> a = input;
    std::vector<uint32_t> b(total_size);
    
    uint32_t* src = a.data();
    uint32_t* dst = b.data();
    
    for (int step = 0; step < steps; ++step) {
        // Copy first row (boundary)
        #pragma omp parallel for schedule(static)
        for (int col = 0; col < cols; ++col) {
            dst[col] = src[col];
        }
        
        // Process interior rows
        #pragma omp parallel for schedule(static)
        for (int row = 1; row < rows - 1; ++row) {
            const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
            const std::size_t prev_row = row_offset - cols_sz;
            const std::size_t next_row = row_offset + cols_sz;
            
            // Left boundary
            dst[row_offset] = src[row_offset];
            
            // Interior columns - main computation
            #pragma omp simd
            for (int col = 1; col < cols - 1; ++col) {
                const std::size_t idx = row_offset + static_cast<std::size_t>(col);
                dst[idx] = (src[idx] * 4u + 
                           src[idx - 1] + 
                           src[idx + 1] + 
                           src[prev_row + static_cast<std::size_t>(col)] + 
                           src[next_row + static_cast<std::size_t>(col)]) / 8u;
            }
            
            // Right boundary
            dst[row_offset + cols_sz - 1] = src[row_offset + cols_sz - 1];
        }
        
        // Copy last row (boundary)
        const std::size_t last_row_offset = static_cast<std::size_t>(rows - 1) * cols_sz;
        #pragma omp parallel for schedule(static)
        for (int col = 0; col < cols; ++col) {
            dst[last_row_offset + col] = src[last_row_offset + col];
        }
        
        // Swap pointers
        uint32_t* tmp = src;
        src = dst;
        dst = tmp;
    }
    
    // Compute checksum - FNV-1a hash
    // Must be sequential due to hash dependency, but we can still optimize memory access
    uint64_t hash = 1469598103934665603ULL;
    const uint32_t* final_data = src;
    
    for (std::size_t i = 0; i < total_size; ++i) {
        hash ^= static_cast<uint64_t>(final_data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}