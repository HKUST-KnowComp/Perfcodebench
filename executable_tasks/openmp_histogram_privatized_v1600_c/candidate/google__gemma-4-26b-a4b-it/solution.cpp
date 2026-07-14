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
  uint64_t hash = 1469598103934665603ULL;
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
  // We use a flat vector to store all thread-local histograms to ensure contiguous memory
  std::vector<uint32_t> local_histograms(n_threads * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts for this iteration
    std::fill(global_counts.begin(), global_counts.end(), 0u);
    // Reset all local buffers
    std::fill(local_histograms.begin(), local_histograms.end(), 0u);

    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* my_hist = &local_histograms[tid * bins];

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input_size; ++i) {
        my_hist[static_cast<std::size_t>(data_ptr[i])]++;
      }
    }

    // Merge local histograms into global counts
    for (int t = 0; t < n_threads; ++t) {
      const uint32_t* local_ptr = &local_histograms[t * bins];
      for (int b = 0; b < bins; ++b) {
        global_counts[b] += local_ptr[b];
      }
    }
  }

  return checksum_u32(global_counts);
}
