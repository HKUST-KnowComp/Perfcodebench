#include "interface.h"

#include <omp.h>
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
  omp_set_num_threads(num_threads());

  const int threads = num_threads();
  const std::size_t bins_sz = static_cast<std::size_t>(bins);
  std::vector<uint32_t> counts(bins_sz, 0u);

  if (bins <= 0) {
    return checksum_u32(counts);
  }

  // Padded per-thread histograms to reduce false sharing during merge/reset.
  const std::size_t stride = ((bins_sz + 15u) / 16u) * 16u;
  std::vector<uint32_t> local(static_cast<std::size_t>(threads) * stride, 0u);

  const uint8_t* in = input.empty() ? nullptr : input.data();
  const std::size_t n = input.size();

  for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * stride;

      for (std::size_t b = 0; b < bins_sz; ++b) hist[b] = 0u;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        ++hist[static_cast<unsigned>(in[i])];
      }

#pragma omp for schedule(static)
      for (std::size_t b = 0; b < bins_sz; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < threads; ++t) {
          sum += local[static_cast<std::size_t>(t) * stride + b];
        }
        counts[b] = sum;
      }
    }
  }

  return checksum_u32(counts);
}
