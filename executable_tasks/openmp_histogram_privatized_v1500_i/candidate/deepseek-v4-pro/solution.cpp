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
  const int N_threads = num_threads();
  omp_set_num_threads(N_threads);
  const std::size_t num_bins = static_cast<std::size_t>(bins);
  std::vector<uint32_t> counts(num_bins, 0);
  std::vector<uint32_t> local_counts(N_threads * num_bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    std::fill(local_counts.begin(), local_counts.end(), 0u);

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* my_counts = local_counts.data() + tid * num_bins;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        my_counts[static_cast<std::size_t>(input[i])]++;
      }
    }

    // Merge thread-local histograms
    for (int t = 0; t < N_threads; ++t) {
      const uint32_t* src = local_counts.data() + t * num_bins;
      for (std::size_t b = 0; b < num_bins; ++b) {
        counts[b] += src[b];
      }
    }
  }

  return checksum_u32(counts);
}