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
  int nthreads = num_threads();
  std::size_t bins_sz = static_cast<std::size_t>(bins);
  std::vector<uint32_t> counts(bins_sz, 0);
  std::vector<uint32_t> tcounts_all(nthreads * bins_sz, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(tcounts_all.begin(), tcounts_all.end(), 0);

#pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      int tid = omp_get_thread_num();
      uint32_t* my_counts = &tcounts_all[tid * bins_sz];
      my_counts[input[i]]++;
    }

    // Combine per-thread histograms
    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < nthreads; ++t) {
        sum += tcounts_all[t * bins_sz + b];
      }
      counts[b] = sum;
    }
  }

  return checksum_u32(counts);
}
