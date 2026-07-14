#include "interface.h"
#include <omp.h>
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
  if (steps == 0) {
    return checksum_u32(input);
  }

  const std::size_t nrows = static_cast<std::size_t>(rows);
  const std::size_t ncols = static_cast<std::size_t>(cols);
  const std::size_t stride = ncols;

  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = a;

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static) default(shared)
    for (std::size_t row = 1; row < nrows - 1; ++row) {
      const std::size_t row_base = row * stride;
      for (std::size_t col = 1; col < ncols - 1; ++col) {
        const std::size_t idx = row_base + col;
        b[idx] = (a[idx] * 4u + a[idx-1] + a[idx+1] + a[idx - stride] + a[idx + stride]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}