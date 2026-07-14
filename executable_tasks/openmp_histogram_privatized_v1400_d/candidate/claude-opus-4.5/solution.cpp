#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

namespace {

static constexpr int kCacheLineSize = 64;
static constexpr int kBins = 256;
// Pad each thread's histogram to avoid false sharing
static constexpr int kPaddedBins = (kBins * sizeof(uint32_t) + kCacheLineSize - 1) / kCacheLineSize * kCacheLineSize / sizeof(uint32_t);

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

uint64_t checksum_u32(const uint32_t* data, int n) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < n; ++i) {
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
  
  // Allocate thread-private histograms with padding
  std::vector<uint32_t> all_counts(static_cast<std::size_t>(nthreads) * kPaddedBins, 0);
  
  // Final histogram
  alignas(64) uint32_t counts[kBins];
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all thread-private histograms
    std::memset(all_counts.data(), 0, all_counts.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict local_counts = all_counts.data() + static_cast<std::size_t>(tid) * kPaddedBins;
      
      // Each thread processes its chunk
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local_counts[data[i]]++;
      }
    }
    
    // Reduce all thread histograms into final counts
    std::memset(counts, 0, sizeof(counts));
    for (int t = 0; t < nthreads; ++t) {
      const uint32_t* __restrict tc = all_counts.data() + static_cast<std::size_t>(t) * kPaddedBins;
      for (int b = 0; b < bins; ++b) {
        counts[b] += tc[b];
      }
    }
  }
  
  return checksum_u32(counts, bins);
}