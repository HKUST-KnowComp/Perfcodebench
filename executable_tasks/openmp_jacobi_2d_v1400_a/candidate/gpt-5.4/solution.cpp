#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = data.size();
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  if (total == 0) {
    return checksum_u32(input);
  }

  std::vector<uint32_t> a = input;
  if (steps <= 0 || rows <= 1 || cols <= 1) {
    return checksum_u32(a);
  }

  std::vector<uint32_t> b(total);

  const int last_row = rows - 1;
  const int last_col = cols - 1;
  const std::size_t cols_sz = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Copy top and bottom rows.
    const std::size_t top_base = 0;
    const std::size_t bottom_base = static_cast<std::size_t>(last_row) * cols_sz;
    for (int c = 0; c < cols; ++c) {
      const std::size_t cc = static_cast<std::size_t>(c);
      b[top_base + cc] = a[top_base + cc];
      b[bottom_base + cc] = a[bottom_base + cc];
    }

    if (rows > 2) {
#pragma omp parallel for schedule(static)
      for (int r = 1; r < last_row; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * cols_sz;
        const std::size_t up = base - cols_sz;
        const std::size_t down = base + cols_sz;

        b[base] = a[base];

        for (int c = 1; c < last_col; ++c) {
          const std::size_t idx = base + static_cast<std::size_t>(c);
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[up + static_cast<std::size_t>(c)] +
                    a[down + static_cast<std::size_t>(c)]) /
                   8u;
        }

        b[base + static_cast<std::size_t>(last_col)] = a[base + static_cast<std::size_t>(last_col)];
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
