#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  const std::size_t n = data.size();
  const uint32_t* __restrict ptr = data.data();
  
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* __restrict pa = a.data();
  uint32_t* __restrict pb = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy top and bottom boundary rows
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      pb[col] = pa[col];
      pb[(rows - 1) * cols_sz + col] = pa[(rows - 1) * cols_sz + col];
    }
    
    // Process interior rows (including left/right boundaries)
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_off = static_cast<std::size_t>(row) * cols_sz;
      const std::size_t row_above = row_off - cols_sz;
      const std::size_t row_below = row_off + cols_sz;
      
      // Left boundary
      pb[row_off] = pa[row_off];
      
      // Interior cells - main computation
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = row_off + static_cast<std::size_t>(col);
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] +
                   pa[row_above + col] + pa[row_below + col]) / 8u;
      }
      
      // Right boundary
      pb[row_off + cols_sz - 1] = pa[row_off + cols_sz - 1];
    }
    
    // Swap pointers
    uint32_t* tmp = pa;
    pa = pb;
    pb = tmp;
  }
  
  // Ensure we return checksum of the correct buffer
  if (steps % 2 == 0) {
    return checksum_u32(a);
  } else {
    return checksum_u32(b);
  }
}