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
  constexpr int PADDING = CACHE_LINE / sizeof(uint32_t);
  const int padded_bins = bins + PADDING;
  std::vector<uint32_t> local_hists(nt * padded_bins, 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts and local histograms
    std::fill(counts.begin(), counts.end(), 0u);
    std::fill(local_hists.begin(), local_hists.end(), 0u);
    
    // Build thread-local histograms
    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local_hist = &local_hists[tid * padded_bins];
      
      #pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < input.size(); ++i) {
        local_hist[input[i]]++;
      }
    }
    
    // Merge thread-local histograms into global histogram
    #pragma omp parallel for schedule(static)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nt; ++t) {
        sum += local_hists[t * padded_bins + b];
      }
      counts[b] = sum;
    }
  }
  
  return checksum_u32(counts);
}