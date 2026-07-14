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
  std::vector<uint32_t> b(a.size(), 0);
  
  const std::size_t n_cols = static_cast<std::size_t>(cols);
  const std::size_t n_rows = static_cast<std::size_t>(rows);

  for (int step = 0; step < steps; ++step) {
    // Handle boundaries and interior separately to remove branching from the hot loop
    #pragma omp parallel
    {
      // 1. Copy boundaries (Rows 0 and Rows-1, and Cols 0 and Cols-1)
      // For simplicity and correctness, we can process the whole grid but optimize the interior.
      // However, the most efficient way is to handle the interior in a tight loop.
      
      #pragma omp for schedule(static)
      for (std::size_t r = 0; r < n_rows; ++r) {
        std::size_t row_offset = r * n_cols;
        if (r == 0 || r == n_rows - 1) {
          // Entire boundary row
          for (std::size_t c = 0; c < n_cols; ++c) {
            b[row_offset + c] = a[row_offset + c];
          }
        } else {
          // Boundary columns for this row
          b[row_offset] = a[row_offset];
          b[row_offset + n_cols - 1] = a[row_offset + n_cols - 1];

          // Interior of the row
          #pragma omp simd
          for (std::size_t c = 1; c < n_cols - 1; ++c) {
            std::size_t idx = row_offset + c;
            b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                      a[idx - n_cols] + a[idx + n_cols]) / 8u;
          }
        }
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
