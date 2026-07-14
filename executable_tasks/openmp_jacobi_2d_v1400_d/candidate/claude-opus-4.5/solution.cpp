#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t n = input.size();
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(n);
  
  uint32_t* src = a.data();
  uint32_t* dst = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy first row (boundary)
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      dst[col] = src[col];
    }
    
    // Copy last row (boundary)
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      const std::size_t idx = static_cast<std::size_t>(rows - 1) * cols_sz + static_cast<std::size_t>(col);
      dst[idx] = src[idx];
    }
    
    // Process interior rows (including left/right boundaries)
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      const uint32_t* src_row = src + row_offset;
      const uint32_t* src_row_up = src + row_offset - cols_sz;
      const uint32_t* src_row_down = src + row_offset + cols_sz;
      uint32_t* dst_row = dst + row_offset;
      
      // Left boundary
      dst_row[0] = src_row[0];
      
      // Interior points - main computation
      for (int col = 1; col < cols - 1; ++col) {
        dst_row[col] = (src_row[col] * 4u + 
                        src_row[col - 1] + src_row[col + 1] +
                        src_row_up[col] + src_row_down[col]) / 8u;
      }
      
      // Right boundary
      dst_row[cols - 1] = src_row[cols - 1];
    }
    
    // Swap pointers
    uint32_t* tmp = src;
    src = dst;
    dst = tmp;
  }
  
  // Compute checksum (FNV-1a variant)
  // Use parallel reduction for large arrays
  const uint32_t* final_data = src;
  
  // Sequential checksum to match exact reference behavior
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(final_data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}