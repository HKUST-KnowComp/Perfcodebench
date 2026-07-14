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
  int num_threads = num_threads();
  omp_set_num_threads(num_threads);
  std::vector<uint32_t> counts(bins, 0);
  std::vector<uint32_t> thread_histograms(num_threads * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(thread_histograms.begin(), thread_histograms.end(), 0);

    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* local = &thread_histograms[tid * bins];

      #pragma omp for
      for (std::size_t i = 0; i < input.size(); ++i) {
        local[input[i]]++;
      }
    }

    for (int t = 0; t < num_threads; ++t) {
      for (int b = 0; b < bins; ++b) {
        counts[b] += thread_histograms[t * bins + b];
      }
    }
  }

  return checksum_u32(counts);
}