#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  std::vector<uint32_t> all_counts(static_cast<std::size_t>(nthreads) * bins, 0);

  const uint8_t* data = input.data();
  std::size_t n = input.size();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    std::fill(all_counts.begin(), all_counts.end(), 0u);

#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* local = &all_counts[static_cast<std::size_t>(tid) * bins];
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        local[data[i]]++;
      }
    }

    for (int t = 0; t < nthreads; ++t) {
      const uint32_t* local = &all_counts[static_cast<std::size_t>(t) * bins];
      for (int b = 0; b < bins; ++b) {
        counts[b] += local[b];
      }
    }
  }
  return checksum_u32(counts);
}
