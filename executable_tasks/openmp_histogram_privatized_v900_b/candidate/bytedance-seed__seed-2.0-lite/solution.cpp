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
  // Preallocate per-thread private histograms to avoid repeated allocations
  std::vector<std::vector<uint32_t>> per_thread_counts(nthreads, std::vector<uint32_t>(bins, 0));

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      // Reset this thread's private histogram for the current iteration
      std::fill(per_thread_counts[tid].begin(), per_thread_counts[tid].end(), 0u);

      // Process input chunk with no atomic overhead, private counts are cache-local
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        const std::size_t val = static_cast<std::size_t>(input[i]);
        per_thread_counts[tid][val]++;
      }

      // Sum private histogram to global counts once per thread, minimal synchronization
#pragma omp critical
      for (std::size_t b = 0; b < static_cast<std::size_t>(bins); ++b) {
        counts[b] += per_thread_counts[tid][b];
      }
    }
  }
  return checksum_u32(counts);
}