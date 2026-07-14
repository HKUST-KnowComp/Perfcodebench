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
  const int nt = num_threads();
  omp_set_num_threads(nt);
  
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  const std::size_t n = input.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    
    std::vector<std::vector<uint32_t>> thread_hists(nt, std::vector<uint32_t>(bins, 0));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      std::vector<uint32_t>& local_hist = thread_hists[tid];
      
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local_hist[input[i]]++;
      }
    }
    
    for (int t = 0; t < nt; ++t) {
      for (int b = 0; b < bins; ++b) {
        counts[b] += thread_hists[t][b];
      }
    }
  }
  
  return checksum_u32(counts);
}
