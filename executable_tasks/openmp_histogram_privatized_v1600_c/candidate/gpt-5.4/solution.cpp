#include "interface.h"

#include <omp.h>
#include <cstdint>
#include <vector>

namespace {

inline int num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) return 1;
  if (t > 8) return 8;
  return t;
}

inline uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  const int threads = num_threads();
  omp_set_num_threads(threads);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  if (bins <= 0) return checksum_u32(counts);

  const std::size_t n = input.size();

  // Fast path for the benchmarked case: 256 bins over uint8_t input.
  if (bins == 256) {
    std::vector<uint32_t> local(static_cast<std::size_t>(threads) * 256u);

    for (int iter = 0; iter < iters; ++iter) {
      for (int t = 0; t < threads; ++t) {
        uint32_t* h = local.data() + static_cast<std::size_t>(t) * 256u;
        for (int b = 0; b < 256; ++b) h[b] = 0u;
      }

#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * 256u;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++hist[input[i]];
        }
      }

      for (int b = 0; b < 256; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < threads; ++t) {
          sum += local[static_cast<std::size_t>(t) * 256u + static_cast<std::size_t>(b)];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }

    return checksum_u32(counts);
  }

  // Generic path for arbitrary bin counts.
  std::vector<uint32_t> local(static_cast<std::size_t>(threads) * static_cast<std::size_t>(bins));

  for (int iter = 0; iter < iters; ++iter) {
    for (int t = 0; t < threads; ++t) {
      uint32_t* h = local.data() + static_cast<std::size_t>(t) * static_cast<std::size_t>(bins);
      for (int b = 0; b < bins; ++b) h[b] = 0u;
    }

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        const std::size_t idx = static_cast<std::size_t>(input[i]);
        if (idx < static_cast<std::size_t>(bins)) {
          ++hist[idx];
        }
      }
    }

    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0u;
      for (int t = 0; t < threads; ++t) {
        sum += local[static_cast<std::size_t>(t) * static_cast<std::size_t>(bins) + static_cast<std::size_t>(b)];
      }
      counts[static_cast<std::size_t>(b)] = sum;
    }
  }

  return checksum_u32(counts);
}
