#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

namespace {

constexpr int CACHE_LINE = 64;
constexpr int BINS = 256;
// Pad each thread's histogram to avoid false sharing
constexpr int PADDED_BINS = ((BINS * sizeof(uint32_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint32_t);

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
  
  // Allocate thread-private histograms with padding to avoid false sharing
  // Using aligned allocation for better cache behavior
  std::vector<uint32_t> all_counts(static_cast<size_t>(nthreads) * PADDED_BINS, 0);
  std::vector<uint32_t> counts(static_cast<size_t>(bins), 0);
  
  const uint8_t* __restrict__ data = input.data();
  const size_t n = input.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all private histograms
    std::memset(all_counts.data(), 0, all_counts.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict__ local_counts = all_counts.data() + static_cast<size_t>(tid) * PADDED_BINS;
      
#pragma omp for schedule(static) nowait
      for (size_t i = 0; i < n; ++i) {
        local_counts[data[i]]++;
      }
    }
    
    // Reduce all thread-private histograms into final counts
    std::memset(counts.data(), 0, static_cast<size_t>(bins) * sizeof(uint32_t));
    for (int t = 0; t < nthreads; ++t) {
      const uint32_t* __restrict__ tc = all_counts.data() + static_cast<size_t>(t) * PADDED_BINS;
      for (int b = 0; b < bins; ++b) {
        counts[static_cast<size_t>(b)] += tc[b];
      }
    }
  }
  
  return checksum_u32(counts.data(), bins);
}