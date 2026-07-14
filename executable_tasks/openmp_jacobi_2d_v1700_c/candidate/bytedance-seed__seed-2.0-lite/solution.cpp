#include "interface.h"
#include <omp.h>
#include <cstring>
#include <cstdint>
#include <vector>

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
  const size_t nrows = static_cast<size_t>(rows);
  const size_t ncols = static_cast<size_t>(cols);
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);

  // Initialize fixed boundaries once, they never change across all iterations
  std::memcpy(b.data(), a.data(), ncols * sizeof(uint32_t));
  std::memcpy(b.data() + (nrows - 1) * ncols, a.data() + (nrows - 1) * ncols, ncols * sizeof(uint32_t));
  for (size_t row = 1; row < nrows - 1; ++row) {
    const size_t row_base = row * ncols;
    b[row_base] = a[row_base];
    b[row_base + ncols - 1] = a[row_base + ncols - 1];
  }

  for (int step = 0; step < steps; ++step) {
    // Parallelize inner row loop with static scheduling for uniform balanced workload
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_base = static_cast<size_t>(row) * ncols;
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_base + static_cast<size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[idx - ncols] + a[idx + ncols]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}