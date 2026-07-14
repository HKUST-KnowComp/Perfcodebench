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
  const std::size_t total = input.size();
  if (total == 0) return checksum_u32(input);
  if (steps <= 0) return checksum_u32(input);

  std::vector<uint32_t> a = input;

  if (rows <= 0 || cols <= 0) {
    return checksum_u32(a);
  }

  if (rows == 1 || cols == 1) {
    return checksum_u32(a);
  }

  std::vector<uint32_t> b(total);
  const std::size_t scols = static_cast<std::size_t>(cols);
  const int last_row = rows - 1;
  const int last_col = cols - 1;

  for (int step = 0; step < steps; ++step) {
    // Copy top and bottom rows.
    #pragma omp parallel
    {
      #pragma omp for schedule(static)
      for (int c = 0; c < cols; ++c) {
        const std::size_t idx = static_cast<std::size_t>(c);
        b[idx] = a[idx];
      }

      #pragma omp for schedule(static)
      for (int c = 0; c < cols; ++c) {
        const std::size_t idx = static_cast<std::size_t>(last_row) * scols + static_cast<std::size_t>(c);
        b[idx] = a[idx];
      }

      // Process interior rows and copy side boundaries.
      #pragma omp for schedule(static)
      for (int r = 1; r < last_row; ++r) {
        const std::size_t row_base = static_cast<std::size_t>(r) * scols;
        b[row_base] = a[row_base];
        b[row_base + static_cast<std::size_t>(last_col)] = a[row_base + static_cast<std::size_t>(last_col)];

        const std::size_t up = row_base - scols;
        const std::size_t down = row_base + scols;

        for (int c = 1; c < last_col; ++c) {
          const std::size_t idx = row_base + static_cast<std::size_t>(c);
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[up + static_cast<std::size_t>(c)] +
                    a[down + static_cast<std::size_t>(c)]) /
                   8u;
        }
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
