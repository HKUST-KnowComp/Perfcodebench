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
    
    // Process interior rows in parallel
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      uint32_t* __restrict__ dst_row = dst + row_offset;
      const uint32_t* __restrict__ src_row = src + row_offset;
      const uint32_t* __restrict__ src_row_up = src + row_offset - cols_sz;
      const uint32_t* __restrict__ src_row_down = src + row_offset + cols_sz;
      
      // Left boundary
      dst_row[0] = src_row[0];
      
      // Interior columns - vectorizable
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        dst_row[col] = (src_row[col] * 4u + 
                        src_row[col - 1] + src_row[col + 1] +
                        src_row_up[col] + src_row_down[col]) / 8u;
      }
      
      // Right boundary
      dst_row[cols - 1] = src_row[cols - 1];
    }
    
    // Copy last row (boundary)
    const std::size_t last_row_offset = static_cast<std::size_t>(rows - 1) * cols_sz;
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      dst[last_row_offset + col] = src[last_row_offset + col];
    }
    
    a.swap(b);
  }
  
  return checksum_u32(a);
}