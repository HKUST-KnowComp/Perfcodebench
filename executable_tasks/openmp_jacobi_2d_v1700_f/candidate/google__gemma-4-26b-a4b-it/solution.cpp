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

} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (rows <= 0 || cols <= 0) return 0;
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  // Pre-copy boundaries to b to ensure they remain constant
  // and handle the case where rows/cols < 3
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 || c == 0 || r == rows - 1 || c == cols - 1) {
        b[r * cols + c] = a[r * cols + c];
      }
    }
  }

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      const std::size_t row_offset = static_cast<std::size_t>(r) * cols;
      const std::size_t prev_row_offset = row_offset - cols;
      const std::size_t next_row_offset = row_offset + cols;

      // Handle boundaries of the row
      b[row_offset] = a[row_offset];
      b[row_offset + cols - 1] = a[row_offset + cols - 1];

      // Main computational kernel
      #pragma omp simd
      for (int c = 1; c < cols - 1; ++c) {
        const std::size_t idx = row_offset + c;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                  a[prev_row_offset + c] + a[next_row_offset + c]) / 8u;
      }
    }
    
    // Ensure boundaries are preserved in b for the next iteration
    // (Though they are copied from a, which is b from previous step)
    // The logic above handles the interior and row-ends. 
    // We must ensure the first and last rows are also correct.
    #pragma omp parallel
    {
      #pragma omp single
      {
        for (int c = 0; c < cols; ++c) {
          b[c] = a[c];
          b[static_cast<std::size_t>(rows - 1) * cols + c] = a[static_cast<std::size_t>(rows - 1) * cols + c];
        }
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}