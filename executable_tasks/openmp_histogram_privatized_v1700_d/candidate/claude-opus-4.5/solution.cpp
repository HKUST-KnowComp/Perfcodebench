#include "interface.h"

#include <algorithm>
#include <cstring>
#include <omp.h>
#include <vector>

namespace {

constexpr int CACHE_LINE = 64;
constexpr int PADDED_BINS = 256 + (CACHE_LINE / sizeof(uint32_t));

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
  
  // Allocate padded thread-local histograms to avoid false sharing
  std::vector<uint32_t> all_local(static_cast<size_t>(nthreads) * PADDED_BINS, 0);
  std::vector<uint32_t> counts(static_cast<size_t>(bins), 0);
  
  const uint8_t* __restrict__ data = input.data();
  const size_t n = input.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear all local histograms
    std::memset(all_local.data(), 0, all_local.size() * sizeof(uint32_t));
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict__ local = all_local.data() + static_cast<size_t>(tid) * PADDED_BINS;
      
      // Compute chunk boundaries for this thread
      const size_t chunk = (n + nthreads - 1) / nthreads;
      const size_t start = static_cast<size_t>(tid) * chunk;
      const size_t end = std::min(start + chunk, n);
      
      // Process 8 elements at a time for better throughput
      size_t i = start;
      const size_t end8 = start + ((end - start) / 8) * 8;
      
      for (; i < end8; i += 8) {
        local[data[i]]++;
        local[data[i + 1]]++;
        local[data[i + 2]]++;
        local[data[i + 3]]++;
        local[data[i + 4]]++;
        local[data[i + 5]]++;
        local[data[i + 6]]++;
        local[data[i + 7]]++;
      }
      
      // Handle remainder
      for (; i < end; ++i) {
        local[data[i]]++;
      }
    }
    
    // Parallel reduction of thread-local histograms
#pragma omp parallel for schedule(static)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nthreads; ++t) {
        sum += all_local[static_cast<size_t>(t) * PADDED_BINS + b];
      }
      counts[b] = sum;
    }
  }
  
  return checksum_u32(counts.data(), bins);
}