#include "interface.h"
#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

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
  int n_threads = num_threads();
  omp_set_num_threads(n_threads);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  
  // Allocate thread-local buffers to avoid atomic contention
  std::vector<uint32_t> local_counts(static_cast<std::size_t>(n_threads) * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    std::fill(local_counts.begin(), local_counts.end(), 0u);

#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* my_counts = &local_counts[tid * bins];
      
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        my_counts[input[i]]++;
      }
    }

    // Merge local buffers into global counts
    for (int t = 0; t < n_threads; ++t) {
      for (int b = 0; b < bins; ++b) {
        counts[b] += local_counts[t * bins + b];
      }
    }
  }
  return checksum_u32(counts);
}