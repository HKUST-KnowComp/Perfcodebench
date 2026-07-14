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
  if (rows <= 0 || cols <= 0) return 0;
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = input;
  
  const size_t stride = static_cast<size_t>(cols);
  
  for (int step = 0; step < steps; ++step) {
    uint32_t* __restrict__ src = a.data();
    uint32_t* __restrict__ dst = b.data();

    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * stride;
      const size_t prev_row = row_offset - stride;
      const size_t next_row = row_offset + stride;
      
      // Handle first column of the row
      dst[row_offset] = src[row_offset];
      
      // Inner loop for columns 1 to cols-2
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + 
                    src[prev_row + col] + src[next_row + col]) / 8u;
      }
      
      // Handle last column of the row
      dst[row_offset + stride - 1] = src[row_offset + stride - 1];
    }

    // Boundary rows (0 and rows-1) are already handled by initializing b = input
    // and not modifying them in the loop. However, since we swap, we must ensure
    // the boundaries are preserved. Since b was initialized as a copy of input,
    // and only the interior is updated, the boundaries remain correct.
    
    a.swap(b);
  }
  
  return checksum_u32(a);
}