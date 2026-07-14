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
  omp_set_num_threads(num_threads());
  const int T = num_threads();
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  std::vector<uint32_t> local_histograms(T * static_cast<std::size_t>(bins), 0);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset thread-local histograms
    std::fill(local_histograms.begin(), local_histograms.end(), 0u);

    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* my_hist = &local_histograms[tid * static_cast<std::size_t>(bins)];

      #pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < input.size(); ++i) {
        my_hist[static_cast<std::size_t>(input[i])]++;
      }
    }

    // Merge private histograms into global counts (serial, but small)
    for (int t = 0; t < T; ++t) {
      const uint32_t* src = &local_histograms[t * static_cast<std::size_t>(bins)];
      for (int b = 0; b < bins; ++b) {
        counts[b] += src[b];
      }
    }
  }

  return checksum_u32(counts);
}
