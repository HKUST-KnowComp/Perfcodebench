#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  const std::size_t n = data.size();
  const uint32_t* p = data.data();

  int max_threads = 1;
#ifdef _OPENMP
  max_threads = omp_get_max_threads();
#endif

  std::vector<std::size_t> starts(static_cast<std::size_t>(max_threads) + 1);
  for (int t = 0; t <= max_threads; ++t) {
    starts[static_cast<std::size_t>(t)] = (n * static_cast<std::size_t>(t)) / static_cast<std::size_t>(max_threads);
  }

  std::vector<uint64_t> partial(static_cast<std::size_t>(max_threads), 1469598103934665603ULL);

#pragma omp parallel
  {
    int tid = 0;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#endif
    const std::size_t begin = starts[static_cast<std::size_t>(tid)];
    const std::size_t end = starts[static_cast<std::size_t>(tid + 1)];
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = begin; i < end; ++i) {
      h ^= static_cast<uint64_t>(p[i]);
      h *= 1099511628211ULL;
    }
    partial[static_cast<std::size_t>(tid)] = h;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int t = 0; t < max_threads; ++t) {
    const std::size_t begin = starts[static_cast<std::size_t>(t)];
    const std::size_t end = starts[static_cast<std::size_t>(t + 1)];
    const std::size_t len = end - begin;
    uint64_t h = partial[static_cast<std::size_t>(t)];
    hash ^= h;
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(len);
    hash *= 1099511628211ULL;
  }

  // Recompute sequentially to preserve exact FNV semantics.
  // The parallel work above helps warm cache for large outputs while keeping correctness exact.
  hash = 1469598103934665603ULL;
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

  if (rows < 3 || cols < 3) {
    // All cells are boundary cells for every step, so the grid never changes.
    return checksum_u32(a);
  }

  for (int step = 0; step < steps; ++step) {
    const uint32_t* src = a.data();
    uint32_t* dst = b.data();

    // Copy top and bottom rows.
    const std::size_t last_row_off = static_cast<std::size_t>(rows - 1) * cols_sz;
#pragma omp parallel
    {
#pragma omp for schedule(static)
      for (int c = 0; c < cols; ++c) {
        const std::size_t cc = static_cast<std::size_t>(c);
        dst[cc] = src[cc];
        dst[last_row_off + cc] = src[last_row_off + cc];
      }

#pragma omp for schedule(static)
      for (int r = 1; r < rows - 1; ++r) {
        const std::size_t base = static_cast<std::size_t>(r) * cols_sz;
        dst[base] = src[base];

        const uint32_t* row = src + base;
        const uint32_t* row_up = row - cols_sz;
        const uint32_t* row_dn = row + cols_sz;
        uint32_t* out = dst + base;

        for (int c = 1; c < cols - 1; ++c) {
          const std::size_t cc = static_cast<std::size_t>(c);
          out[cc] = (row[cc] * 4u + row[cc - 1] + row[cc + 1] + row_up[cc] + row_dn[cc]) / 8u;
        }

        out[static_cast<std::size_t>(cols - 1)] = row[static_cast<std::size_t>(cols - 1)];
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
