#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

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
  
  // Pad each thread's histogram to avoid false sharing (64-byte cache lines)
  // 256 bins * 4 bytes = 1024 bytes, pad to 1024 + 64 = 1088 bytes = 272 uint32_t
  constexpr int PADDED_BINS = 272;
  
  // Allocate thread-local histograms
  std::vector<uint32_t> local_hists(static_cast<std::size_t>(nthreads) * PADDED_BINS, 0);
  
  // Final histogram
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  
  const std::size_t n = input.size();
  const uint8_t* __restrict data = input.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all local histograms
    std::memset(local_hists.data(), 0, local_hists.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict my_hist = local_hists.data() + static_cast<std::size_t>(tid) * PADDED_BINS;
      
      // Each thread processes a contiguous chunk
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        my_hist[data[i]]++;
      }
    }
    
    // Reduce all thread-local histograms into counts
    std::memset(counts.data(), 0, static_cast<std::size_t>(bins) * sizeof(uint32_t));
    
#pragma omp parallel for schedule(static)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nthreads; ++t) {
        sum += local_hists[static_cast<std::size_t>(t) * PADDED_BINS + static_cast<std::size_t>(b)];
      }
      counts[static_cast<std::size_t>(b)] = sum;
    }
  }
  
  return checksum_u32(counts.data(), bins);
}