#include "interface.h"

#include <algorithm>
#include <cstdint>
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

  // Allocate per-thread histograms (one per thread, re-initialized each iteration)
  std::vector<std::vector<uint32_t>> per_thread_counts(nthreads, std::vector<uint32_t>(bins, 0));

  std::vector<uint32_t> counts(bins, 0);
  const std::size_t len = input.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset per-thread histograms (already zeroed from initialization, but we fill for safety)
    for (int t = 0; t < nthreads; ++t) {
      std::fill(per_thread_counts[t].begin(), per_thread_counts[t].end(), 0u);
    }

#pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < len; ++i) {
      int tid = omp_get_thread_num();
      per_thread_counts[tid][static_cast<std::size_t>(input[i])]++;
    }

    // Combine per-thread histograms into single counts
    std::fill(counts.begin(), counts.end(), 0u);
    for (int t = 0; t < nthreads; ++t) {
      const auto& thread_hist = per_thread_counts[t];
      for (int b = 0; b < bins; ++b) {
        counts[b] += thread_hist[b];
      }
    }
  }

  return checksum_u32(counts);
}
