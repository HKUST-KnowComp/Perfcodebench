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
    // Handle boundaries and core separately to avoid branching in the hot loop
    #pragma omp parallel
    {
      // 1. Handle the first and last rows (boundaries)
      #pragma omp for
      for (std::size_t col = 0; col < n_cols; ++col) {
        b[col] = a[col]; // row 0
        b[(n_rows - 1) * n_cols + col] = a[(n_rows - 1) * n_cols + col]; // last row
      }

      // 2. Handle the first and last columns (boundaries) for middle rows
      #pragma omp for
      for (std::size_t row = 1; row < n_rows - 1; ++row) {
        b[row * n_cols] = a[row * n_cols]; // col 0
        b[row * n_cols + (n_cols - 1)] = a[row * n_cols + (n_cols - 1)]; // last col
      }

      // 3. The core computation (Jacobi smoothing)
      #pragma omp for
      for (std::size_t row = 1; row < n_rows - 1; ++row) {
        const std::size_t row_offset = row * n_cols;
        const std::size_t prev_row_offset = (row - 1) * n_cols;
        const std::size_t next_row_offset = (row + 1) * n_cols;

        // The inner loop is now branchless and highly vectorizable
        #pragma omp simd
        for (std::size_t col = 1; col < n_cols - 1; ++col) {
          const std::size_t idx = row_offset + col;
          b[idx] = (a[idx] * 4u + 
                    a[idx - 1] + 
                    a[idx + 1] + 
                    a[prev_row_offset + col] + 
                    a[next_row_offset + col]) / 8u;
        }
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
