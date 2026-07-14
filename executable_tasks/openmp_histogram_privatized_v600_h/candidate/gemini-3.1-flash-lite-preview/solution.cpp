#include "interface.h"
#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

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
  const size_t n = input.size();
  const uint8_t* data = input.data();
  std::vector<uint32_t> global_counts(static_cast<size_t>(bins), 0);

  int max_threads = omp_get_max_threads();
  std::vector<uint32_t> private_counts(static_cast<size_t>(max_threads) * 256, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);
    std::fill(private_counts.begin(), private_counts.end(), 0u);

#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* local_bin = &private_counts[tid * 256];

#pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_bin[data[i]]++;
      }
    }

    for (int t = 0; t < max_threads; ++t) {
      for (int b = 0; b < 256; ++b) {
        global_counts[b] += private_counts[t * 256 + b];
      }
    }
  }
  return checksum_u32(global_counts);
}