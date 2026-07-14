#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>
#include <cstring>

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
  // Early exit for trivial cases where no updates are possible
  if (steps == 0 || rows < 3 || cols < 3) {
    return checksum_u32(input);
  }

  const size_t cols_sz = static_cast<size_t>(cols);
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);

  // Copy immutable boundaries to both grids once (they never change)
  // Copy top and bottom full rows
  memcpy(b.data(), input.data(), cols_sz * sizeof(uint32_t));
  memcpy(b.data() + static_cast<size_t>(rows - 1) * cols_sz, 
         input.data() + static_cast<size_t>(rows - 1) * cols_sz, 
         cols_sz * sizeof(uint32_t));
  // Copy left/right boundaries for all middle rows
  for (int row = 1; row < rows - 1; ++row) {
    const size_t base = static_cast<size_t>(row) * cols_sz;
    b[base] = input[base];
    b[base + cols - 1] = input[base + cols - 1];
  }

  // Run Jacobi steps
  for (int step = 0; step < steps; ++step) {
    // Parallelize inner row processing across threads
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t base = static_cast<size_t>(row) * cols_sz;
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = base + col;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                  a[idx - cols_sz] + a[idx + cols_sz]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}