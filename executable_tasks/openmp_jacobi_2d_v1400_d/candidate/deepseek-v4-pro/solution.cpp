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
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);

  for (int step = 0; step < steps; ++step) {
    // Interior points: independent rows, OpenMP parallel
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      std::size_t row_start = static_cast<std::size_t>(row) * cols;
      for (int col = 1; col < cols - 1; ++col) {
        std::size_t idx = row_start + col;
        b[idx] = (a[idx] * 4u +
                  a[idx - 1] +
                  a[idx + 1] +
                  a[idx - static_cast<std::size_t>(cols)] +
                  a[idx + static_cast<std::size_t>(cols)]) /
                 8u;
      }
    }

    // Top and bottom rows (boundary copy)
    for (int col = 0; col < cols; ++col) {
      b[col] = a[col];
      b[(rows - 1) * static_cast<std::size_t>(cols) + col] =
          a[(rows - 1) * static_cast<std::size_t>(cols) + col];
    }

    // Left and right columns (excluding corners already copied)
    for (int row = 1; row < rows - 1; ++row) {
      std::size_t idx_left = static_cast<std::size_t>(row) * cols;
      std::size_t idx_right = idx_left + cols - 1;
      b[idx_left] = a[idx_left];
      b[idx_right] = a[idx_right];
    }

    a.swap(b);
  }

  return checksum_u32(a);
}