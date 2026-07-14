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
  if (total == 0 || rows <= 0 || cols <= 0 || steps <= 0) {
    return checksum_u32(input);
  }

  std::vector<uint32_t> a = input;

  if (rows < 3 || cols < 3) {
    return checksum_u32(a);
  }

  std::vector<uint32_t> b(total);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  const int last_row = rows - 1;
  const int last_col = cols - 1;

  for (int step = 0; step < steps; ++step) {
    // Copy top and bottom rows.
    #pragma omp parallel
    {
      #pragma omp for schedule(static)
      for (int c = 0; c < cols; ++c) {
        const std::size_t idx_top = static_cast<std::size_t>(c);
        const std::size_t idx_bot = static_cast<std::size_t>(last_row) * cols_sz + static_cast<std::size_t>(c);
        b[idx_top] = a[idx_top];
        b[idx_bot] = a[idx_bot];
      }

      // Compute interior and copy left/right borders for each interior row.
      #pragma omp for schedule(static)
      for (int r = 1; r < last_row; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * cols_sz;
        b[base] = a[base];
        b[base + static_cast<std::size_t>(last_col)] = a[base + static_cast<std::size_t>(last_col)];

        const uint32_t* prev = a.data() + base - cols_sz;
        const uint32_t* curr = a.data() + base;
        const uint32_t* next = a.data() + base + cols_sz;
        uint32_t* out = b.data() + base;

        #pragma omp simd
        for (int c = 1; c < last_col; ++c) {
          out[c] = (curr[c] * 4u + curr[c - 1] + curr[c + 1] + prev[c] + next[c]) / 8u;
        }
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
