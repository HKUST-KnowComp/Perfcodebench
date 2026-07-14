#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const uint32_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (rows <= 0 || cols <= 0) return 0;
  
  size_t size = static_cast<size_t>(rows) * cols;
  std::vector<uint32_t> buf1 = input;
  std::vector<uint32_t> buf2(size);
  
  uint32_t* a = buf1.data();
  uint32_t* b = buf2.data();

  // Initialize boundaries for b since they are copied from a every step
  // To optimize, we can just copy the boundary once and keep them constant
  #pragma omp parallel for
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 || c == 0 || r == rows - 1 || c == cols - 1) {
        b[static_cast<size_t>(r) * cols + c] = a[static_cast<size_t>(r) * cols + c];
      }
    }
  }

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * cols;
      const size_t prev_row = row_offset - cols;
      const size_t next_row = row_offset + cols;
      
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                  a[prev_row + col] + a[next_row + col]) / 8u;
      }
    }
    // Swap pointers
    uint32_t* temp = a;
    a = b;
    b = temp;
  }

  return checksum_u32(a, size);
}