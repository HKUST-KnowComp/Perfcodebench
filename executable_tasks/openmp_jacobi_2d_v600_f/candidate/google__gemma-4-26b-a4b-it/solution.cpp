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
  
  const size_t n_rows = static_cast<size_t>(rows);
  const size_t n_cols = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Handle boundaries and core separately to remove branching from the hot loop
    #pragma omp parallel
    {
      // 1. Handle the boundary rows (top and bottom)
      #pragma omp for nowait
      for (size_t col = 0; col < n_cols; ++col) {
        b[col] = a[col]; // Top row
        b[(n_rows - 1) * n_cols + col] = a[(n_rows - 1) * n_cols + col]; // Bottom row
      }

      // 2. Handle the boundary columns (left and right) for middle rows
      #pragma omp for nowait
      for (size_t row = 1; row < n_rows - 1; ++row) {
        size_t idx = row * n_cols;
        b[idx] = a[idx]; // Left col
        b[idx + n_cols - 1] = a[idx + n_cols - 1]; // Right col
      }

      // 3. The Core: Jacobi smoothing
      #pragma omp for
      for (size_t row = 1; row < n_rows - 1; ++row) {
        const size_t row_offset = row * n_cols;
        const size_t prev_row_offset = (row - 1) * n_cols;
        const size_t next_row_offset = (row + 1) * n_cols;
        
        for (size_t col = 1; col < n_cols - 1; ++col) {
          const size_t idx = row_offset + col;
          // b[idx] = (a[idx]*4 + a[idx-1] + a[idx+1] + a[idx-cols] + a[idx+cols]) / 8
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