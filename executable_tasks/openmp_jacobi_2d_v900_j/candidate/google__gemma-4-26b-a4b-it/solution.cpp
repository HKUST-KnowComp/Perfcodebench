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
  if (input.empty()) return 0;

  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  const size_t total_size = static_cast<size_t>(rows) * cols;
  const size_t c_size = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Handle boundaries separately to keep the inner loop clean for vectorization
    // 1. Top and Bottom rows
    #pragma omp parallel for
    for (int col = 0; col < cols; ++col) {
      b[col] = a[col];
      b[static_cast<size_t>(rows - 1) * c_size + col] = a[static_cast<size_t>(rows - 1) * c_size + col];
    }

    // 2. Left and Right columns (excluding corners already handled)
    #pragma omp parallel for
    for (int row = 1; row < rows - 1; ++row) {
      b[static_cast<size_t>(row) * c_size] = a[static_cast<size_t>(row) * c_size];
      b[static_cast<size_t>(row) * c_size + (cols - 1)] = a[static_cast<size_t>(row) * c_size + (cols - 1)];
    }

    // 3. Core computation
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * c_size;
      const size_t prev_row_offset = row_offset - c_size;
      const size_t next_row_offset = row_offset + c_size;

      // The compiler can vectorize this loop easily because there are no branches
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        b[idx] = (a[idx] * 4u + 
                  a[idx - 1] + 
                  a[idx + 1] + 
                  a[prev_row_offset + col] + 
                  a[next_row_offset + col]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
