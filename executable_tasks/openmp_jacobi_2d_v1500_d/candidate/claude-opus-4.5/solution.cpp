#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* src = a.data();
  uint32_t* dst = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (top and bottom)
    #pragma omp parallel
    {
      #pragma omp for nowait
      for (std::size_t col = 0; col < cols_sz; ++col) {
        dst[col] = src[col];
      }
      
      #pragma omp for nowait
      for (std::size_t col = 0; col < cols_sz; ++col) {
        std::size_t idx = (rows - 1) * cols_sz + col;
        dst[idx] = src[idx];
      }
      
      // Process interior rows (including left/right boundary columns)
      #pragma omp for schedule(static)
      for (int row = 1; row < rows - 1; ++row) {
        const std::size_t row_off = static_cast<std::size_t>(row) * cols_sz;
        const std::size_t row_above = row_off - cols_sz;
        const std::size_t row_below = row_off + cols_sz;
        
        // Left boundary
        dst[row_off] = src[row_off];
        
        // Interior columns - main computation
        for (std::size_t col = 1; col < cols_sz - 1; ++col) {
          const std::size_t idx = row_off + col;
          dst[idx] = (src[idx] * 4u + 
                      src[idx - 1] + src[idx + 1] +
                      src[row_above + col] + src[row_below + col]) / 8u;
        }
        
        // Right boundary
        dst[row_off + cols_sz - 1] = src[row_off + cols_sz - 1];
      }
    }
    
    // Swap pointers
    uint32_t* tmp = src;
    src = dst;
    dst = tmp;
  }
  
  // Compute checksum - parallelize with reduction
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  
  // Sequential checksum to match reference exactly
  uint64_t hash = FNV_OFFSET;
  for (std::size_t i = 0; i < total; ++i) {
    hash ^= static_cast<uint64_t>(src[i]);
    hash *= FNV_PRIME;
  }
  
  return hash;
}