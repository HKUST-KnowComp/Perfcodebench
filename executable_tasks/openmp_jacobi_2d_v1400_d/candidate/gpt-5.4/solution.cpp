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
  const std::size_t total = input.size();
  if (total == 0) {
    return checksum_u32_ptr(nullptr, 0);
  }

  std::vector<uint32_t> a = input;
  if (steps <= 0 || rows <= 0 || cols <= 0) {
    return checksum_u32_ptr(a.data(), a.size());
  }

  std::vector<uint32_t> b(total);

  const int r = rows;
  const int c = cols;

  if (r <= 2 || c <= 2) {
    for (int step = 0; step < steps; ++step) {
      a.swap(b);
    }
    return checksum_u32_ptr((steps & 1) ? b.data() : a.data(), total);
  }

  uint32_t* ap = a.data();
  uint32_t* bp = b.data();

  for (int step = 0; step < steps; ++step) {
    const std::size_t cols_sz = static_cast<std::size_t>(c);
    const std::size_t last_row = static_cast<std::size_t>(r - 1) * cols_sz;

    for (int j = 0; j < c; ++j) {
      const std::size_t idx = static_cast<std::size_t>(j);
      bp[idx] = ap[idx];
      bp[last_row + idx] = ap[last_row + idx];
    }

    #pragma omp parallel for schedule(static)
    for (int i = 1; i < r - 1; ++i) {
      const std::size_t base = static_cast<std::size_t>(i) * cols_sz;
      const std::size_t up = base - cols_sz;
      const std::size_t down = base + cols_sz;

      bp[base] = ap[base];
      for (int j = 1; j < c - 1; ++j) {
        const std::size_t idx = base + static_cast<std::size_t>(j);
        bp[idx] = (ap[idx] * 4u + ap[idx - 1] + ap[idx + 1] + ap[up + static_cast<std::size_t>(j)] + ap[down + static_cast<std::size_t>(j)]) / 8u;
      }
      bp[base + static_cast<std::size_t>(c - 1)] = ap[base + static_cast<std::size_t>(c - 1)];
    }

    uint32_t* tmp = ap;
    ap = bp;
    bp = tmp;
  }

  return checksum_u32_ptr(ap, total);
}
