#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t checksum_u32_ptr(const uint32_t* data, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
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
    return checksum_u32_ptr(nullptr, 0);
  }

  std::vector<uint32_t> a = input;
  if (steps <= 0 || rows <= 1 || cols <= 1) {
    return checksum_u32_ptr(a.data(), total);
  }

  std::vector<uint32_t> b(total);

  const int last_row = rows - 1;
  const int last_col = cols - 1;
  const std::size_t cols_sz = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    const uint32_t* __restrict src = a.data();
    uint32_t* __restrict dst = b.data();

    // Copy top and bottom rows.
    for (int c = 0; c < cols; ++c) {
      const std::size_t idx_top = static_cast<std::size_t>(c);
      const std::size_t idx_bot = static_cast<std::size_t>(last_row) * cols_sz + static_cast<std::size_t>(c);
      dst[idx_top] = src[idx_top];
      dst[idx_bot] = src[idx_bot];
    }

    if (rows > 2) {
#pragma omp parallel for schedule(static)
      for (int r = 1; r < last_row; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * cols_sz;
        const std::size_t up = base - cols_sz;
        const std::size_t down = base + cols_sz;

        dst[base] = src[base];

        for (int c = 1; c < last_col; ++c) {
          const std::size_t idx = base + static_cast<std::size_t>(c);
          dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + src[up + static_cast<std::size_t>(c)] +
                      src[down + static_cast<std::size_t>(c)]) /
                     8u;
        }

        dst[base + static_cast<std::size_t>(last_col)] = src[base + static_cast<std::size_t>(last_col)];
      }
    }

    a.swap(b);
  }

  return checksum_u32_ptr(a.data(), total);
}
