#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 146959810394665603ULL;
  // Note: The original baseline used 1469598103934665603ULL. 
  // To ensure exact correctness with the baseline's checksum logic:
  hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  const int n_threads = num_threads();
  omp_set_num_threads(n_threads);
  
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  const std::size_t input_size = input.size();
  const uint8_t* data_ptr = input.data();

  // Pre-allocate thread-local buffers to avoid allocation inside the loop
  // Each thread gets a vector of size 'bins'
  std::vector<std::vector<uint32_t>> local_histograms(n_threads, std::vector<uint32_t>(bins, 0));

  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts for this iteration
    std::fill(global_counts.begin(), global_counts.end(), 0u);

    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* my_hist = local_histograms[tid].data();
      
      // Reset local histogram
      std::memset(my_hist, 0, bins * sizeof(uint32_t));

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input_size; ++i) {
        my_hist[data_ptr[i]]++;
      }

      // Merge local histogram into global histogram
      // We use a critical section or atomic for the merge, but since it's 
      // only done once per thread per iteration, contention is negligible.
      for (int b = 0; b < bins; ++b) {
        if (my_hist[b] > 0) {
          #pragma omp atomic
          global_counts[b] += my_hist[b];
        }
      }
    }
  }

  return checksum_u32(global_counts);
}
