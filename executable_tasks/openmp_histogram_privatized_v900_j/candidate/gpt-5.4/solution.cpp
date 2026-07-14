#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>

namespace {

inline int num_threads() {
  int mt = omp_get_max_threads();
  if (mt < 1) return 1;
  return mt > 8 ? 8 : mt;
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
  omp_set_num_threads(num_threads());

  const std::size_t bins_sz = static_cast<std::size_t>(bins);
  std::vector<uint32_t> counts(bins_sz, 0u);
  if (iters <= 0) return checksum_u32(counts);

  const std::size_t n = input.size();
  const uint8_t* in = input.data();

  // Fast path for the benchmarked case: 256 bins over uint8_t input.
  if (bins == 256) {
    const int max_t = num_threads();
    const std::size_t stride = 256 + 16;  // pad to reduce false sharing
    std::vector<uint32_t> local(static_cast<std::size_t>(max_t) * stride);

    for (int iter = 0; iter < iters; ++iter) {
      std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * stride;
        for (int b = 0; b < 256; ++b) hist[b] = 0u;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++hist[in[i]];
        }

#pragma omp for schedule(static)
        for (int b = 0; b < 256; ++b) {
          uint32_t sum = 0;
          for (int t = 0; t < max_t; ++t) {
            sum += local[static_cast<std::size_t>(t) * stride + static_cast<std::size_t>(b)];
          }
          counts[static_cast<std::size_t>(b)] = sum;
        }
      }
    }

    return checksum_u32(counts);
  }

  // Generic fallback for arbitrary bin counts.
  const int max_t = num_threads();
  const std::size_t pad = 16;
  const std::size_t stride = bins_sz + pad;
  std::vector<uint32_t> local(static_cast<std::size_t>(max_t) * stride);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * stride;
      for (std::size_t b = 0; b < bins_sz; ++b) hist[b] = 0u;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        ++hist[static_cast<std::size_t>(in[i])];
      }

#pragma omp for schedule(static)
      for (std::size_t b = 0; b < bins_sz; ++b) {
        uint32_t sum = 0;
        for (int t = 0; t < max_t; ++t) {
          sum += local[static_cast<std::size_t>(t) * stride + b];
        }
        counts[b] = sum;
      }
    }
  }

  return checksum_u32(counts);
}
