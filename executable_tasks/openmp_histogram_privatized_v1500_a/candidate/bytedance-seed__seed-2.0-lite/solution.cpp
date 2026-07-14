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
  // Preallocate per-thread private histograms once to avoid repeated allocations
  std::vector<std::vector<uint32_t>> thread_counts(nthreads, std::vector<uint32_t>(bins, 0));

  for (int iter = 0; iter < iters; ++iter) {
    // Reset all counts for the current iteration
    std::fill(counts.begin(), counts.end(), 0u);
    for (auto& local_counts : thread_counts) {
      std::fill(local_counts.begin(), local_counts.end(), 0u);
    }

    // Process input with zero cache contention, no atomics in hot path
#pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      const int tid = omp_get_thread_num();
      thread_counts[tid][static_cast<std::size_t>(input[i])]++;
    }

    // Merge all thread-local histograms into the global count array
    for (int t = 0; t < nthreads; ++t) {
      for (int b = 0; b < bins; ++b) {
        counts[b] += thread_counts[t][b];
      }
    }
  }
  return checksum_u32(counts);
}