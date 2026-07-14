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
  
  // Padded histogram size to avoid false sharing (64 bytes = cache line)
  constexpr int CACHE_LINE = 64;
  constexpr int PAD = CACHE_LINE / sizeof(uint32_t);  // 16 uint32_t per cache line
  const int padded_bins = ((bins + PAD - 1) / PAD) * PAD;
  
  // Thread-local histograms with padding between them
  std::vector<uint32_t> local_hists(static_cast<size_t>(nthreads) * padded_bins, 0);
  
  // Final histogram
  std::vector<uint32_t> counts(static_cast<size_t>(bins), 0);
  
  const uint8_t* __restrict data = input.data();
  const size_t n = input.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear local histograms
    std::memset(local_hists.data(), 0, local_hists.size() * sizeof(uint32_t));
    
    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* __restrict my_hist = local_hists.data() + static_cast<size_t>(tid) * padded_bins;
      
      // Compute chunk boundaries for this thread
      const size_t chunk_size = (n + nthreads - 1) / nthreads;
      const size_t start = static_cast<size_t>(tid) * chunk_size;
      const size_t end = std::min(start + chunk_size, n);
      
      // Process 4 elements at a time for better ILP
      size_t i = start;
      const size_t end4 = start + ((end - start) / 4) * 4;
      
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
    
    // Parallel reduction of local histograms into final counts
    #pragma omp parallel for schedule(static)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nthreads; ++t) {
        sum += local_hists[static_cast<size_t>(t) * padded_bins + b];
      }
      counts[b] = sum;
    }
  }
  
  return checksum_u32(counts.data(), bins);
}