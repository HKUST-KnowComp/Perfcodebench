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
    // Handle boundaries first (they remain constant in value from 'a' to 'b')
    // Top and bottom rows
    #pragma omp parallel for
    for (std::size_t c = 0; c < n_cols; ++c) {
      b[c] = a[c];
      b[(n_rows - 1) * n_cols + c] = a[(n_rows - 1) * n_cols + c];
    }

    // Left and right columns (excluding corners already handled)
    #pragma omp parallel for
    for (std::size_t r = 1; r < n_rows - 1; ++r) {
      b[r * n_cols] = a[r * n_cols];
      b[r * n_cols + (n_cols - 1)] = a[r * n_cols + (n_cols - 1)];
    }

    // Main computation loop for the interior
    #pragma omp parallel for schedule(static)
    for (std::size_t r = 1; r < n_rows - 1; ++r) {
      const std::size_t row_offset = r * n_cols;
      for (std::size_t c = 1; c < n_cols - 1; ++c) {
        const std::size_t idx = row_offset + c;
        // Jacobi update formula: (4*center + left + right + up + down) / 8
        b[idx] = (a[idx] * 4u + 
                   a[idx - 1] + 
                   a[idx + 1] + 
                   a[idx - n_cols] + 
                   a[idx + n_cols]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}