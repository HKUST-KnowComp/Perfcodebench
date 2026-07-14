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
  std::vector<uint32_t> b(a.size());

  const int nrows = rows;
  const int ncols = cols;

  for (int step = 0; step < steps; ++step) {
    const uint32_t* pa = a.data();
    uint32_t* pb = b.data();

    #pragma omp parallel for schedule(static)
    for (int r = 0; r < nrows; ++r) {
      const std::size_t base = static_cast<std::size_t>(r) * static_cast<std::size_t>(ncols);
      const uint32_t* row_cur = pa + base;
      uint32_t* row_out = pb + base;

      if (r == 0 || r == nrows - 1) {
        // Copy entire boundary row
        for (int c = 0; c < ncols; ++c) {
          row_out[c] = row_cur[c];
        }
      } else {
        // Left boundary column copy
        row_out[0] = row_cur[0];

        // Interior columns update
        const uint32_t* row_prev = pa + base - static_cast<std::size_t>(ncols);
        const uint32_t* row_next = pa + base + static_cast<std::size_t>(ncols);

        // Compute for columns 1..ncols-2
        #pragma omp simd
        for (int c = 1; c < ncols - 1; ++c) {
          // Keep arithmetic in uint32_t to preserve wrap-around semantics
          uint32_t s = row_cur[c] * 4u;
          s += row_cur[c - 1];
          s += row_cur[c + 1];
          s += row_prev[c];
          s += row_next[c];
          row_out[c] = s / 8u;
        }

        // Right boundary column copy
        row_out[static_cast<std::size_t>(ncols) - 1] = row_cur[static_cast<std::size_t>(ncols) - 1];
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
