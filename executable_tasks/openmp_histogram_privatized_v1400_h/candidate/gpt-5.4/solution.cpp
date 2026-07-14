#include "interface.h"

#include <omp.h>
#include <cstdint>
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

  const std::size_t n = input.size();
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  if (bins <= 0) return checksum_u32(counts);

  const int max_t = num_threads();
  std::vector<uint32_t> private_counts(static_cast<std::size_t>(max_t) * static_cast<std::size_t>(bins), 0u);

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0, e = counts.size(); i < e; ++i) counts[i] = 0u;
    for (std::size_t i = 0, e = private_counts.size(); i < e; ++i) private_counts[i] = 0u;

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local = private_counts.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        ++local[input[i]];
      }

#pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0;
        for (int t = 0; t < max_t; ++t) {
          sum += private_counts[static_cast<std::size_t>(t) * static_cast<std::size_t>(bins) + static_cast<std::size_t>(b)];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }
  }

  return checksum_u32(counts);
}
