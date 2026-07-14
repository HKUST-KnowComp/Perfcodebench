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
  int nth = num_threads();
  omp_set_num_threads(nth);

  std::size_t n = input.size();
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  std::vector<uint32_t> priv(static_cast<std::size_t>(nth) * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    // Parallel histogram with privatization
#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      std::size_t start = static_cast<std::size_t>(tid) * bins;

      // Zero private segment
      std::fill(&priv[start], &priv[start] + bins, 0u);

      // Histogram computation
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        priv[start + static_cast<std::size_t>(input[i])]++;
      }
    } // end parallel

    // Combine private histograms into global (serial reduction)
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nth; ++t) {
        sum += priv[static_cast<std::size_t>(t) * bins + b];
      }
      global_counts[static_cast<std::size_t>(b)] = sum;
    }
  }

  return checksum_u32(global_counts);
}
