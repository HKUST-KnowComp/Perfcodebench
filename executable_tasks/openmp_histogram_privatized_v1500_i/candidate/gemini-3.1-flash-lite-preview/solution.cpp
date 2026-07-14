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
  const int num_threads = std::min(omp_get_max_threads(), 64);
  std::vector<uint32_t> global_counts(256, 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);
    std::vector<uint32_t> local_histograms(num_threads * 256, 0);

#pragma omp parallel num_threads(num_threads)
    {
      int tid = omp_get_thread_num();
      uint32_t* my_hist = &local_histograms[tid * 256];
      
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        my_hist[input[i]]++;
      }
    }

    for (int t = 0; t < num_threads; ++t) {
      uint32_t* my_hist = &local_histograms[t * 256];
      for (int b = 0; b < 256; ++b) {
        global_counts[b] += my_hist[b];
      }
    }
  }
  return checksum_u32(global_counts);
}