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
  const int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  // Preallocate per-thread private histograms once to avoid repeated allocation overhead
  std::vector<std::vector<uint32_t>> private_counts(nthreads, std::vector<uint32_t>(bins, 0));

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      // Reset this thread's private histogram for the new iteration
      std::fill(private_counts[tid].begin(), private_counts[tid].end(), 0u);
      // Synchronize to ensure all threads have reset their private bins before processing
      #pragma omp barrier
      // Process input chunk with no atomics: only this thread accesses its private histogram
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        private_counts[tid][static_cast<std::size_t>(input[i])]++;
      }
      // Synchronize to ensure all threads have finished processing input before merging
      #pragma omp barrier
      // Master thread merges all private histograms into the global count array to avoid race conditions
      if (tid == 0) {
        for (int t = 1; t < nthreads; ++t) {
          for (int b = 0; b < bins; ++b) {
            counts[b] += private_counts[t][b];
          }
        }
      }
    }
  }
  return checksum_u32(counts);
}