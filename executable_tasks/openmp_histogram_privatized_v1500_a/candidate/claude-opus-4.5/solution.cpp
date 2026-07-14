#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

namespace {

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
  const std::size_t n = input.size();
  const uint8_t* __restrict data = input.data();
  
  // For small inputs, avoid OpenMP overhead entirely
  if (n < 4096) {
    alignas(64) uint32_t counts[256] = {0};
    for (int iter = 0; iter < iters; ++iter) {
      if (iter > 0) std::memset(counts, 0, sizeof(counts));
      for (std::size_t i = 0; i < n; ++i) {
        counts[data[i]]++;
      }
    }
    return checksum_u32(counts, bins);
  }
  
  const int max_threads = std::max(1, std::min(8, omp_get_max_threads()));
  omp_set_num_threads(max_threads);
  
  // Allocate private histograms for each thread (cache-line aligned)
  // Using 256 bins per thread, padded to avoid false sharing
  std::vector<uint32_t> all_private(static_cast<std::size_t>(max_threads) * 256, 0);
  alignas(64) uint32_t counts[256] = {0};
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear final counts
    std::memset(counts, 0, sizeof(counts));
    
    // Clear all private histograms
    std::memset(all_private.data(), 0, all_private.size() * sizeof(uint32_t));
    
    // Each thread builds its own private histogram
    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict my_hist = all_private.data() + static_cast<std::size_t>(tid) * 256;
      
      // Determine this thread's chunk
      const int nthreads = omp_get_num_threads();
      const std::size_t chunk = (n + static_cast<std::size_t>(nthreads) - 1) / static_cast<std::size_t>(nthreads);
      const std::size_t start = static_cast<std::size_t>(tid) * chunk;
      const std::size_t end = std::min(start + chunk, n);
      
      // Process 4 elements at a time for better ILP
      std::size_t i = start;
      const std::size_t end4 = start + ((end - start) / 4) * 4;
      for (; i < end4; i += 4) {
        my_hist[data[i]]++;
        my_hist[data[i + 1]]++;
        my_hist[data[i + 2]]++;
        my_hist[data[i + 3]]++;
      }
      // Handle remainder
      for (; i < end; ++i) {
        my_hist[data[i]]++;
      }
    }
    
    // Single-threaded reduction of all private histograms
    const int actual_threads = max_threads;
    for (int t = 0; t < actual_threads; ++t) {
      const uint32_t* __restrict src = all_private.data() + static_cast<std::size_t>(t) * 256;
      for (int b = 0; b < bins; ++b) {
        counts[b] += src[b];
      }
    }
  }
  
  return checksum_u32(counts, bins);
}