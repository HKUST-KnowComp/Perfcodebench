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
  
  size_t size = static_cast<size_t>(rows) * static_cast<size_t>(cols);
  std::vector<uint32_t> buf1 = input;
  std::vector<uint32_t> buf2(size);

  uint32_t* a = buf1.data();
  uint32_t* b = buf2.data();

  // Initialize boundary values for the second buffer once
  #pragma omp parallel for collapse(2)
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 || c == 0 || r == rows - 1 || c == cols - 1) {
        b[static_cast<size_t>(r) * cols + c] = a[static_cast<size_t>(r) * cols + c];
      }
    }
  }

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for
    for (int r = 1; r < rows - 1; ++r) {
      const size_t row_offset = static_cast<size_t>(r) * cols;
      const size_t prev_row = row_offset - cols;
      const size_t next_row = row_offset + cols;
      
      #pragma omp simd
      for (int c = 1; c < cols - 1; ++c) {
        const size_t idx = row_offset + c;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[prev_row + c] + a[next_row + c]) / 8u;
      }
    }
    std::swap(a, b);
  }

  return checksum_u32(a, size);
}