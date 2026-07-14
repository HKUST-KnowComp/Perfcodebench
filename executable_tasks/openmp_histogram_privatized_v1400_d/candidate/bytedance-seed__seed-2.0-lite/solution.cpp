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
  const int num_thr = num_threads();
  omp_set_num_threads(num_thr);
  const std::size_t bin_count = static_cast<std::size_t>(bins);
  const std::size_t input_size = input.size();
  
  std::vector<uint32_t> counts(bin_count, 0);
  std::vector<std::vector<uint32_t>> private_counts(num_thr, std::vector<uint32_t>(bin_count, 0));

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      std::fill(private_counts[tid].begin(), private_counts[tid].end(), 0u);

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input_size; ++i) {
        private_counts[tid][static_cast<std::size_t>(input[i])]++;
      }
    }

    for (int t = 0; t < num_thr; ++t) {
      for (std::size_t b = 0; b < bin_count; ++b) {
        counts[b] += private_counts[t][b];
      }
    }
  }

  return checksum_u32(counts);
}