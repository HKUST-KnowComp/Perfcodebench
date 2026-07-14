#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = data.size();
  const uint32_t* p = data.data();
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  const std::size_t total = a.size();
  if (steps <= 0 || total == 0) {
    return checksum_u32(a);
  }

  std::vector<uint32_t> b(total);

  if (rows <= 0 || cols <= 0) {
    return checksum_u32(a);
  }

  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  const int last_row = rows - 1;
  const int last_col = cols - 1;

  for (int step = 0; step < steps; ++step) {
    const uint32_t* __restrict src = a.data();
    uint32_t* __restrict dst = b.data();

    if (rows == 1 || cols == 1) {
#pragma omp parallel for schedule(static)
      for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(total); ++i) {
        dst[static_cast<std::size_t>(i)] = src[static_cast<std::size_t>(i)];
      }
      a.swap(b);
      continue;
    }

#pragma omp parallel
    {
#pragma omp for schedule(static)
      for (int c = 0; c < cols; ++c) {
        const std::size_t idx_top = static_cast<std::size_t>(c);
        const std::size_t idx_bottom = static_cast<std::size_t>(last_row) * cols_sz + static_cast<std::size_t>(c);
        dst[idx_top] = src[idx_top];
        dst[idx_bottom] = src[idx_bottom];
      }

#pragma omp for schedule(static)
      for (int r = 1; r < last_row; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * cols_sz;
        dst[base] = src[base];
        dst[base + static_cast<std::size_t>(last_col)] = src[base + static_cast<std::size_t>(last_col)];
      }

#pragma omp for schedule(static)
      for (int r = 1; r < last_row; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * cols_sz;
        const std::size_t up = base - cols_sz;
        const std::size_t down = base + cols_sz;
        for (int c = 1; c < last_col; ++c) {
          const std::size_t idx = base + static_cast<std::size_t>(c);
          dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + src[up + static_cast<std::size_t>(c)] +
                      src[down + static_cast<std::size_t>(c)]) /
                     8u;
        }
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
