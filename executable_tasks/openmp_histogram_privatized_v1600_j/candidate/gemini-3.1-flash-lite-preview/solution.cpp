#include "interface.h"
#include <algorithm>
#include <omp.h>
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

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  const int num_threads = omp_get_max_threads();
  std::vector<uint32_t> global_counts(256, 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);
    std::vector<std::vector<uint32_t>> local_counts(num_threads, std::vector<uint32_t>(256, 0));

#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      auto& my_counts = local_counts[tid];
      
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        my_counts[input[i]]++;
      }
    }

    for (int t = 0; t < num_threads; ++t) {
      for (int b = 0; b < 256; ++b) {
        global_counts[b] += local_counts[t][b];
      }
    }
  }
  return checksum_u32(global_counts);
}