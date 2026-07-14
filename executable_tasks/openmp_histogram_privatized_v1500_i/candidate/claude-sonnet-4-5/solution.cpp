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
  
  // Allocate thread-local histograms with padding to prevent false sharing
  constexpr int CACHE_LINE = 64;
  const int padded_bins = ((bins * sizeof(uint32_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint32_t);
  std::vector<uint32_t> thread_hists(nt * padded_bins, 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    std::fill(thread_hists.begin(), thread_hists.end(), 0u);
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local_hist = &thread_hists[tid * padded_bins];
      
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < input.size(); ++i) {
        local_hist[static_cast<std::size_t>(input[i])]++;
      }
    }
    
    // Merge thread-local histograms
    for (int t = 0; t < nt; ++t) {
      const uint32_t* local_hist = &thread_hists[t * padded_bins];
      for (int b = 0; b < bins; ++b) {
        counts[b] += local_hist[b];
      }
    }
  }
  
  return checksum_u32(counts);
}
