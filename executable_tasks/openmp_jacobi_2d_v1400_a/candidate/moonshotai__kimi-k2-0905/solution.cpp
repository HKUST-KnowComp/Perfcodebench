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

  const std::size_t cols_u = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Interior cells (no boundary checks inside the loop)
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t base = static_cast<std::size_t>(row) * cols_u;
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = base + static_cast<std::size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - cols_u] + a[idx + cols_u]) / 8u;
      }
    }

    // Copy border cells verbatim (excluded from the stencil)
    // Top and bottom rows
    for (int col = 0; col < cols; ++col) {
      b[col] = a[col];
      b[(rows - 1) * cols_u + col] = a[(rows - 1) * cols_u + col];
    }
    // Left and right columns (skip corners already copied)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t base = static_cast<std::size_t>(row) * cols_u;
      b[base] = a[base];
      b[base + cols_u - 1] = a[base + cols_u - 1];
    }

    a.swap(b);
  }
  return checksum_u32(a);
}