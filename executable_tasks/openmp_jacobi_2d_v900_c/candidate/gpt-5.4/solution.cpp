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
  if (total == 0 || rows <= 0 || cols <= 0) {
    return checksum_u32(input);
  }

  std::vector<uint32_t> a = input;
  if (steps <= 0) {
    return checksum_u32(a);
  }

  std::vector<uint32_t> b(total);

  const int last_row = rows - 1;
  const int last_col = cols - 1;

  for (int step = 0; step < steps; ++step) {
    if (rows <= 2 || cols <= 2) {
#pragma omp parallel for schedule(static)
      for (int r = 0; r < rows; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * static_cast<std::size_t>(cols);
        for (int c = 0; c < cols; ++c) {
          b[base + static_cast<std::size_t>(c)] = a[base + static_cast<std::size_t>(c)];
        }
      }
    } else {
#pragma omp parallel
      {
#pragma omp for schedule(static)
        for (int c = 0; c < cols; ++c) {
          const std::size_t idx_top = static_cast<std::size_t>(c);
          const std::size_t idx_bottom = static_cast<std::size_t>(last_row) * static_cast<std::size_t>(cols) + static_cast<std::size_t>(c);
          b[idx_top] = a[idx_top];
          b[idx_bottom] = a[idx_bottom];
        }

#pragma omp for schedule(static)
        for (int r = 1; r < last_row; ++r) {
          const std::size_t base = static_cast<std::size_t>(r) * static_cast<std::size_t>(cols);
          b[base] = a[base];
          b[base + static_cast<std::size_t>(last_col)] = a[base + static_cast<std::size_t>(last_col)];
        }

#pragma omp for schedule(static)
        for (int r = 1; r < last_row; ++r) {
          const std::size_t base = static_cast<std::size_t>(r) * static_cast<std::size_t>(cols);
          const std::size_t up = base - static_cast<std::size_t>(cols);
          const std::size_t down = base + static_cast<std::size_t>(cols);

          int c = 1;
          for (; c + 3 < last_col; c += 4) {
            const std::size_t i0 = base + static_cast<std::size_t>(c);
            const std::size_t i1 = i0 + 1;
            const std::size_t i2 = i0 + 2;
            const std::size_t i3 = i0 + 3;

            b[i0] = (a[i0] * 4u + a[i0 - 1] + a[i0 + 1] + a[up + static_cast<std::size_t>(c)] + a[down + static_cast<std::size_t>(c)]) / 8u;
            b[i1] = (a[i1] * 4u + a[i1 - 1] + a[i1 + 1] + a[up + static_cast<std::size_t>(c + 1)] + a[down + static_cast<std::size_t>(c + 1)]) / 8u;
            b[i2] = (a[i2] * 4u + a[i2 - 1] + a[i2 + 1] + a[up + static_cast<std::size_t>(c + 2)] + a[down + static_cast<std::size_t>(c + 2)]) / 8u;
            b[i3] = (a[i3] * 4u + a[i3 - 1] + a[i3 + 1] + a[up + static_cast<std::size_t>(c + 3)] + a[down + static_cast<std::size_t>(c + 3)]) / 8u;
          }
          for (; c < last_col; ++c) {
            const std::size_t idx = base + static_cast<std::size_t>(c);
            b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[up + static_cast<std::size_t>(c)] + a[down + static_cast<std::size_t>(c)]) / 8u;
          }
        }
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
