#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size());
  
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  for (int step = 0; step < steps; ++step) {
    uint32_t* __restrict__ src = a.data();
    uint32_t* __restrict__ dst = b.data();
    
    // Copy first row (boundary)
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      dst[col] = src[col];
    }
    
    // Process interior rows
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      
      // First column (boundary)
      dst[row_offset] = src[row_offset];
      
      // Interior columns
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = row_offset + static_cast<std::size_t>(col);
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] +
                    src[idx - cols_sz] + src[idx + cols_sz]) / 8u;
      }
      
      // Last column (boundary)
      dst[row_offset + cols_sz - 1] = src[row_offset + cols_sz - 1];
    }
    
    // Copy last row (boundary)
    const std::size_t last_row_offset = static_cast<std::size_t>(rows - 1) * cols_sz;
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      dst[last_row_offset + col] = src[last_row_offset + col];
    }
    
    a.swap(b);
  }
  
  // Compute checksum
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* data = a.data();
  const std::size_t n = a.size();
  
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}