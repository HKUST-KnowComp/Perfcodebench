#include "interface.h"
#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

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

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  const int num_threads = omp_get_max_threads();
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  
  // Allocate thread-local buffers to avoid atomic contention
  std::vector<uint32_t> local_counts(static_cast<std::size_t>(num_threads) * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);
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

    // Merge local buffers into global
    for (int t = 0; t < num_threads; ++t) {
      for (int b = 0; b < bins; ++b) {
        global_counts[b] += local_counts[t * bins + b];
      }
    }
  }
  return checksum_u32(global_counts);
}