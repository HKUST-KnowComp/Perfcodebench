#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  // Each thread gets a private histogram. 256 bins * 4 bytes = 1024 bytes per thread,
  // which is a multiple of typical cache line sizes, naturally avoiding false sharing.
  std::vector<uint32_t> local_counts(static_cast<std::size_t>(nthreads) * bins, 0);
  
  const std::size_t N = input.size();
  const uint8_t* __restrict__ data = input.data();

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    uint32_t* __restrict__ my_hist = &local_counts[static_cast<std::size_t>(tid) * bins];
    
    for (int iter = 0; iter < iters; ++iter) {
      // Reset local histogram
      for (int b = 0; b < bins; ++b) {
        my_hist[b] = 0;
      }
      
      // Distribute input chunks statically for cache locality
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < N; ++i) {
        my_hist[data[i]]++;
      }
      // Implicit barrier ensures all threads finish histogram updates
      
      #pragma omp single
      {
        // Fast sequential reduction
        std::fill(counts.begin(), counts.end(), 0u);
        for (int t = 0; t < nthreads; ++t) {
          const uint32_t* __restrict__ t_hist = &local_counts[static_cast<std::size_t>(t) * bins];
          for (int b = 0; b < bins; ++b) {
            counts[b] += t_hist[b];
          }
        }
      }
      // Implicit barrier at end of single ensures reduction completes before next iter
    }
  }
  
  return checksum_u32(counts);
}