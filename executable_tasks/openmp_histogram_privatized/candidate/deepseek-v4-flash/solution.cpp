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
  omp_set_num_threads(num_threads());
  int nt = omp_get_max_threads();
  // Private histogram array: one row per thread, each row of size bins
  std::vector<uint32_t> priv(static_cast<std::size_t>(nt) * bins, 0);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset private histograms
    std::fill(priv.begin(), priv.end(), 0u);
    counts.assign(bins, 0u);

    // Parallel histogram with private per-thread accumulation
    #pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      int tid = omp_get_thread_num();
      priv[static_cast<std::size_t>(tid) * bins + input[i]]++;
    }

    // Reduce private histograms into global counts
    for (int t = 0; t < nt; ++t) {
      std::size_t base = static_cast<std::size_t>(t) * bins;
      for (int b = 0; b < bins; ++b) {
        counts[b] += priv[base + b];
      }
    }
  }

  return checksum_u32(counts);
}
