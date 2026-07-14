#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
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
  const int nt = omp_get_max_threads();
  const std::size_t total_private = static_cast<std::size_t>(nt) * bins;
  std::vector<uint32_t> private_hist(total_private);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

  for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* local = &private_hist[static_cast<std::size_t>(tid) * bins];
      // zero local histogram
      for (int b = 0; b < bins; ++b) {
        local[b] = 0;
      }
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        ++local[input[i]];
      }
    }
    // combine private histograms into counts
#pragma omp parallel for
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nt; ++t) {
        sum += private_hist[static_cast<std::size_t>(t) * bins + b];
      }
      counts[static_cast<std::size_t>(b)] = sum;
    }
  }

  return checksum_u32(counts);
}
