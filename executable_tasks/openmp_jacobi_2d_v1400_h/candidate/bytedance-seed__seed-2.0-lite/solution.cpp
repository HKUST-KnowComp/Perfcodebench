#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>

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
  std::vector<uint32_t> b(a.size(), 0);
  
  const int s_rows = rows;
  const int s_cols = cols;
  const std::size_t sz_cols = static_cast<std::size_t>(s_cols);

  // Copy fixed grid boundaries once, they never change across all steps
  if (s_rows > 0 && s_cols > 0) {
    // Copy top row (row 0)
    for (int col = 0; col < s_cols; ++col) {
      const std::size_t idx = static_cast<std::size_t>(col);
      b[idx] = a[idx];
    }
    // Copy bottom row (last row)
    const std::size_t bottom_offset = static_cast<std::size_t>(s_rows - 1) * sz_cols;
    for (int col = 0; col < s_cols; ++col) {
      const std::size_t idx = bottom_offset + static_cast<std::size_t>(col);
      b[idx] = a[idx];
    }
    // Copy left/right boundaries for middle rows
    for (int row = 1; row < s_rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * sz_cols;
      b[row_offset] = a[row_offset];
      b[row_offset + static_cast<std::size_t>(s_cols - 1)] = a[row_offset + static_cast<std::size_t>(s_cols - 1)];
    }
  }

  for (int step = 0; step < steps; ++step) {
    // Parallelize inner grid updates across all available threads
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < s_rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * sz_cols;
      for (int col = 1; col < s_cols - 1; ++col) {
        const std::size_t idx = row_offset + static_cast<std::size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[idx - sz_cols] + a[idx + sz_cols]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}