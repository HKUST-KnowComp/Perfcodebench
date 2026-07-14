#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

constexpr int CACHE_LINE = 64;
constexpr int BINS = 256;
constexpr int PADDED_BINS = ((BINS * sizeof(uint32_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint32_t);

int num_threads() {
  return std::max(1, std::min(omp_get_max_threads(), 16));
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
  
  // Allocate thread-private histograms with padding to avoid false sharing
  std::vector<uint32_t> all_local(static_cast<std::size_t>(nthreads) * PADDED_BINS, 0);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all local histograms
    std::memset(all_local.data(), 0, all_local.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict local = all_local.data() + static_cast<std::size_t>(tid) * PADDED_BINS;
      
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        local[data[i]]++;
      }
    }
    
    // Reduce all thread-local histograms into counts
    std::memset(counts.data(), 0, static_cast<std::size_t>(bins) * sizeof(uint32_t));
    
#pragma omp parallel for schedule(static)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nthreads; ++t) {
        sum += all_local[static_cast<std::size_t>(t) * PADDED_BINS + static_cast<std::size_t>(b)];
      }
      counts[static_cast<std::size_t>(b)] = sum;
    }
  }
  
  return checksum_u32(counts.data(), bins);
}