#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(16, omp_get_max_threads()));
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
  
  // Padded histogram size to avoid false sharing (64-byte cache lines)
  constexpr int CACHE_LINE = 64;
  constexpr int PAD = CACHE_LINE / sizeof(uint32_t); // 16 uint32_t per cache line
  const int padded_bins = ((bins + PAD - 1) / PAD) * PAD;
  
  // Allocate thread-local histograms with padding between them
  std::vector<uint32_t> all_local(static_cast<size_t>(nthreads) * padded_bins, 0);
  
  // Final histogram
  std::vector<uint32_t> counts(static_cast<size_t>(bins), 0);
  
  const size_t n = input.size();
  const uint8_t* __restrict data = input.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all local histograms
    std::memset(all_local.data(), 0, all_local.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict local = all_local.data() + static_cast<size_t>(tid) * padded_bins;
      
#pragma omp for schedule(static) nowait
      for (size_t i = 0; i < n; ++i) {
        local[data[i]]++;
      }
    }
    
    // Reduce all thread-local histograms into counts
    std::memset(counts.data(), 0, counts.size() * sizeof(uint32_t));
    
#pragma omp parallel for schedule(static)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nthreads; ++t) {
        sum += all_local[static_cast<size_t>(t) * padded_bins + b];
      }
      counts[b] = sum;
    }
  }
  
  return checksum_u32(counts.data(), bins);
}