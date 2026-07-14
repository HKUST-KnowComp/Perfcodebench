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
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  if (total == 0) return checksum_u32(input);
  if (steps <= 0) return checksum_u32(input);

  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);

  if (rows <= 2 || cols <= 2) {
    for (int step = 0; step < steps; ++step) {
#pragma omp parallel for schedule(static)
      for (std::int64_t i = 0; i < static_cast<std::int64_t>(total); ++i) {
        b[static_cast<std::size_t>(i)] = a[static_cast<std::size_t>(i)];
      }
      a.swap(b);
    }
    return checksum_u32(a);
  }

  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  const std::size_t last_row_base = static_cast<std::size_t>(rows - 1) * cols_sz;

#pragma omp parallel
  {
    for (int step = 0; step < steps; ++step) {
#pragma omp for schedule(static)
      for (std::int64_t c = 0; c < cols; ++c) {
        const std::size_t col = static_cast<std::size_t>(c);
        b[col] = a[col];
        b[last_row_base + col] = a[last_row_base + col];
      }

#pragma omp for schedule(static)
      for (std::int64_t r = 1; r < rows - 1; ++r) {
        const std::size_t row = static_cast<std::size_t>(r);
        const std::size_t base = row * cols_sz;
        b[base] = a[base];
        b[base + cols_sz - 1] = a[base + cols_sz - 1];

        const uint32_t* up = a.data() + base - cols_sz;
        const uint32_t* cur = a.data() + base;
        const uint32_t* down = a.data() + base + cols_sz;
        uint32_t* out = b.data() + base;

        for (std::size_t c = 1; c + 1 < cols_sz; ++c) {
          out[c] = (cur[c] * 4u + cur[c - 1] + cur[c + 1] + up[c] + down[c]) / 8u;
        }
      }

#pragma omp single
      { a.swap(b); }
#pragma omp barrier
    }
  }

  return checksum_u32(a);
}
