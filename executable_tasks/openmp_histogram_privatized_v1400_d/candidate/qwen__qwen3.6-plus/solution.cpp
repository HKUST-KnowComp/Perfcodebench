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
  std::vector<uint32_t> local_counts(static_cast<std::size_t>(nthreads) * bins, 0);

  const uint8_t* data = input.data();
  std::size_t n = input.size();

  #pragma omp parallel num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    uint32_t* my_counts = &local_counts[tid * bins];
    
    for (int iter = 0; iter < iters; ++iter) {
      std::fill(my_counts, my_counts + bins, 0u);
      
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        my_counts[data[i]]++;
      }
      
      if (tid == 0) {
        std::fill(counts.begin(), counts.end(), 0u);
        for (int t = 0; t < nthreads; ++t) {
          const uint32_t* src = &local_counts[t * bins];
          for (int b = 0; b < bins; ++b) {
            counts[b] += src[b];
          }
        }
      }
    }
  }
  
  return checksum_u32(counts);
}
