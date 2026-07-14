#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

uint64_t checksum_u32(const uint32_t* data, int bins) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < bins; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  const int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  
  const std::size_t n = input.size();
  const uint8_t* __restrict data = input.data();
  
  // Padded to cache line (64 bytes = 16 uint32_t)
  constexpr int CACHE_LINE_INTS = 16;
  constexpr int PADDED_BINS = 256 + CACHE_LINE_INTS;
  
  // Allocate private histograms for all threads, cache-line padded
  std::vector<uint32_t> all_private(static_cast<std::size_t>(nthreads) * PADDED_BINS, 0);
  
  // Final histogram
  alignas(64) uint32_t counts[256];
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear private histograms
    std::memset(all_private.data(), 0, all_private.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict local_hist = all_private.data() + static_cast<std::size_t>(tid) * PADDED_BINS;
      
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local_hist[data[i]]++;
      }
    }
    
    // Merge all private histograms into counts
    std::memset(counts, 0, sizeof(counts));
    for (int t = 0; t < nthreads; ++t) {
      const uint32_t* __restrict local_hist = all_private.data() + static_cast<std::size_t>(t) * PADDED_BINS;
      for (int b = 0; b < 256; ++b) {
        counts[b] += local_hist[b];
      }
    }
  }
  
  return checksum_u32(counts, bins);
}