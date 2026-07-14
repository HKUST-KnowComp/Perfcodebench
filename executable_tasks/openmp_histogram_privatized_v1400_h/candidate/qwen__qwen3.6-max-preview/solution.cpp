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
  omp_set_dynamic(0);
  omp_set_num_threads(nthreads);
  
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  std::vector<uint32_t> thread_counts(static_cast<std::size_t>(nthreads) * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      int actual_nthreads = omp_get_num_threads();
      uint32_t* local = &thread_counts[tid * bins];
      
      for (int b = 0; b < bins; ++b) local[b] = 0;

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        local[input[i]]++;
      }

      #pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0;
        for (int t = 0; t < actual_nthreads; ++t) {
          sum += thread_counts[t * bins + b];
        }
        counts[b] = sum;
      }
    }
  }
  return checksum_u32(counts);
}
