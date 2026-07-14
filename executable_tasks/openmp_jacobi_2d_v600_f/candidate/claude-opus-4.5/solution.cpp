#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* __restrict pa = a.data();
  uint32_t* __restrict pb = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (top and bottom)
    #pragma omp parallel for schedule(static)
    for (std::size_t col = 0; col < cols_sz; ++col) {
      pb[col] = pa[col];
      pb[(rows - 1) * cols_sz + col] = pa[(rows - 1) * cols_sz + col];
    }
    
    // Process interior rows in parallel
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      const std::size_t prev_row = row_offset - cols_sz;
      const std::size_t next_row = row_offset + cols_sz;
      
      // Copy left boundary
      pb[row_offset] = pa[row_offset];
      
      // Process interior columns with SIMD hints
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = row_offset + static_cast<std::size_t>(col);
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] +
                   pa[prev_row + col] + pa[next_row + col]) / 8u;
      }
      
      // Copy right boundary
      pb[row_offset + cols_sz - 1] = pa[row_offset + cols_sz - 1];
    }
    
    // Swap pointers
    uint32_t* tmp = pa;
    pa = pb;
    pb = tmp;
  }
  
  // Compute checksum in parallel using reduction
  // FNV-1a hash computed sequentially for correctness
  const uint32_t* result = pa;
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < total; ++i) {
    hash ^= static_cast<uint64_t>(result[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}