#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

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
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  const size_t n = input.size();
  const uint8_t* data = input.data();

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int num_t = omp_get_num_threads();
    
    // Privatize the histogram for each thread
    std::vector<uint32_t> local_counts(bins, 0);

    for (int iter = 0; iter < iters; ++iter) {
      // Reset local counts for each iteration
      std::fill(local_counts.begin(), local_counts.end(), 0u);

      #pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_counts[data[i]]++;
      }

      // Merge local counts into global counts
      // To avoid contention during merge, we can use a critical section or atomics
      // Since this happens once per iteration per thread, atomics are fine here
      for (int b = 0; b < bins; ++b) {
        #pragma omp atomic
        global_counts[b] += local_counts[b];
      }
    }
  }

  // The problem statement implies the checksum is calculated on the final counts
  // after 'iters' iterations. However, the baseline resets counts every iteration.
  // Re-reading baseline: it fills counts with 0 inside the iters loop.
  // This means the checksum is only based on the LAST iteration's results.
  
  // Correcting logic to match baseline: the baseline resets counts every iteration.
  // Therefore, we only care about the result of the final iteration.
  
  // Let's refine the loop to match the baseline's behavior exactly.
  
  std::vector<uint32_t> final_counts(static_cast<std::size_t>(bins), 0);
  
  #pragma omp parallel
  {
    std::vector<uint32_t> local_counts(bins, 0);
    for (int iter = 0; iter < iters; ++iter) {
      std::fill(local_counts.begin(), local_counts.end(), 0u);
      #pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_counts[data[i]]++;
      }
      
      if (iter == iters - 1) {
        for (int b = 0; b < bins; ++b) {
          #pragma omp atomic
          final_counts[b] += local_counts[b];
        }
      }
    }
  }

  return checksum_u32(final_counts);
}