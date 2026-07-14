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

  const std::size_t r = static_cast<std::size_t>(rows);
  const std::size_t c = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Handle boundaries explicitly to avoid condition checks in interior loop
    // Top and bottom rows (full columns)
    for (std::size_t col = 0; col < c; ++col) {
      b[col] = a[col];
      b[(r - 1) * c + col] = a[(r - 1) * c + col];
    }
    // Left and right columns (excluding first and last rows, already set above)
    for (std::size_t row = 1; row < r - 1; ++row) {
      b[row * c] = a[row * c];
      b[row * c + (c - 1)] = a[row * c + (c - 1)];
    }

    // Interior cells: row 1 to rows-2, col 1 to cols-2
#pragma omp parallel for
    for (std::size_t row = 1; row < r - 1; ++row) {
      const std::size_t row_offset = row * c;
      const std::size_t row_offset_prev = (row - 1) * c;
      const std::size_t row_offset_next = (row + 1) * c;
      for (std::size_t col = 1; col < c - 1; ++col) {
        const std::size_t idx = row_offset + col;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[row_offset_prev + col] + a[row_offset_next + col]) >> 3;
      }
    }

    a.swap(b);
  }
  return checksum_u32(a);
}
