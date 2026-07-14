#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* __restrict src = a.data();
  uint32_t* __restrict dst = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (top and bottom)
    #pragma omp parallel for schedule(static)
    for (std::size_t col = 0; col < cols_sz; ++col) {
      dst[col] = src[col];
      dst[(rows - 1) * cols_sz + col] = src[(rows - 1) * cols_sz + col];
    }
    
    // Process interior rows with boundary columns
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      
      // Left boundary
      dst[row_offset] = src[row_offset];
      // Right boundary
      dst[row_offset + cols_sz - 1] = src[row_offset + cols_sz - 1];
      
      // Interior columns - vectorizable
      #pragma omp simd
      for (std::size_t col = 1; col < cols_sz - 1; ++col) {
        const std::size_t idx = row_offset + col;
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] +
                    src[idx - cols_sz] + src[idx + cols_sz]) / 8u;
      }
    }
    
    // Swap pointers
    uint32_t* tmp = src;
    src = dst;
    dst = tmp;
  }
  
  // Compute checksum - use the correct buffer (src points to result after swaps)
  uint64_t hash = 1469598103934665603ULL;
  
  // Sequential checksum to maintain exact order-dependent hash
  for (std::size_t i = 0; i < total; ++i) {
    hash ^= static_cast<uint64_t>(src[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}