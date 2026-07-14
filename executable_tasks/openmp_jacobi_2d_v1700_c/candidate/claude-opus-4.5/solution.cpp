#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  for (int step = 0; step < steps; ++step) {
    uint32_t* __restrict__ a_ptr = a.data();
    uint32_t* __restrict__ b_ptr = b.data();
    
    // Copy boundary rows (top and bottom)
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      b_ptr[col] = a_ptr[col];
      b_ptr[(rows - 1) * cols_sz + col] = a_ptr[(rows - 1) * cols_sz + col];
    }
    
    // Process interior rows (including left/right boundary columns)
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      const std::size_t prev_row = row_offset - cols_sz;
      const std::size_t next_row = row_offset + cols_sz;
      
      // Left boundary
      b_ptr[row_offset] = a_ptr[row_offset];
      
      // Interior points
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = row_offset + static_cast<std::size_t>(col);
        b_ptr[idx] = (a_ptr[idx] * 4u + a_ptr[idx - 1] + a_ptr[idx + 1] +
                      a_ptr[prev_row + col] + a_ptr[next_row + col]) / 8u;
      }
      
      // Right boundary
      b_ptr[row_offset + cols_sz - 1] = a_ptr[row_offset + cols_sz - 1];
    }
    
    a.swap(b);
  }
  
  return checksum_u32(a);
}