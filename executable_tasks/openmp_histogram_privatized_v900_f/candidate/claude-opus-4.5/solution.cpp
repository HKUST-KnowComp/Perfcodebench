#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

namespace {

static constexpr int kCacheLineSize = 64;
static constexpr int kBins = 256;
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
  
  // Allocate padded thread-local histograms to avoid false sharing
  std::vector<uint32_t> all_counts(static_cast<size_t>(nthreads) * kPaddedBins, 0);
  
  const uint8_t* __restrict data = input.data();
  const size_t n = input.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all thread-local histograms
    std::memset(all_counts.data(), 0, all_counts.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict local_counts = all_counts.data() + static_cast<size_t>(tid) * kPaddedBins;
      
#pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_counts[data[i]]++;
      }
    }
    
    // Reduce thread-local histograms into the first one
    uint32_t* __restrict final_counts = all_counts.data();
    for (int t = 1; t < nthreads; ++t) {
      const uint32_t* __restrict tc = all_counts.data() + static_cast<size_t>(t) * kPaddedBins;
      for (int b = 0; b < bins; ++b) {
        final_counts[b] += tc[b];
      }
    }
  }
  
  return checksum_u32(all_counts.data(), bins);
}