#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstddef>

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
  const int nthreads = num_threads();
  omp_set_num_threads(nthreads);

  // Preallocate private per-thread histograms to avoid repeated allocation
  std::vector<std::vector<uint32_t>> thread_counts(nthreads, std::vector<uint32_t>(static_cast<std::size_t>(bins), 0));
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);

    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      auto& local_counts = thread_counts[tid];
      // Parallel reset of local counts to avoid serial overhead
      std::fill(local_counts.begin(), local_counts.end(), 0u);

      // Process input chunk with zero cross-thread synchronization
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        local_counts[static_cast<std::size_t>(input[i])]++;
      }
    } // Implicit barrier ensures all local updates are complete before merging

    // Merge all thread-local results into global counts (no locks required, all writes finalized)
    for (const auto& local : thread_counts) {
      for (int b = 0; b < bins; ++b) {
        global_counts[static_cast<std::size_t>(b)] += local[static_cast<std::size_t>(b)];
      }
    }
  }

  return checksum_u32(global_counts);
}