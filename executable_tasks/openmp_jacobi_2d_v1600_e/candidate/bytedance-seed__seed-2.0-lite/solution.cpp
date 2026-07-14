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
  const std::size_t nrows = static_cast<std::size_t>(rows);
  const std::size_t ncols = static_cast<std::size_t>(cols);
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);

  // Set grid boundaries once, they never change across all iterations
  if (nrows > 0 && ncols > 0) {
    // Copy first row boundaries
    for (std::size_t col = 0; col < ncols; ++col) {
      b[col] = a[col];
    }
    // Copy last row boundaries
    const std::size_t last_row_offset = (nrows - 1) * ncols;
    for (std::size_t col = 0; col < ncols; ++col) {
      b[last_row_offset + col] = a[last_row_offset + col];
    }
    // Copy first/last column boundaries for middle rows
    for (std::size_t row = 1; row < nrows - 1; ++row) {
      const std::size_t row_offset = row * ncols;
      b[row_offset] = a[row_offset];
      b[row_offset + ncols - 1] = a[row_offset + ncols - 1];
    }
  }

  for (int step = 0; step < steps; ++step) {
    // Process only inner grid cells, boundaries remain permanently correct
    #pragma omp parallel for default(none) shared(a, b, nrows, ncols) schedule(static)
    for (std::size_t row = 1; row < nrows - 1; ++row) {
      const std::size_t row_offset = row * ncols;
      const std::size_t prev_row_offset = (row - 1) * ncols;
      const std::size_t next_row_offset = (row + 1) * ncols;
      #pragma omp simd
      for (std::size_t col = 1; col < ncols - 1; ++col) {
        const std::size_t idx = row_offset + col;
        const uint32_t center = a[idx];
        const uint32_t left = a[idx - 1];
        const uint32_t right = a[idx + 1];
        const uint32_t up = a[prev_row_offset + col];
        const uint32_t down = a[next_row_offset + col];
        b[idx] = (center * 4u + left + right + up + down) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}