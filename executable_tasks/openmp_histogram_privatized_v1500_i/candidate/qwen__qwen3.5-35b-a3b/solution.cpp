#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

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
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    #pragma omp parallel
    {
      std::vector<uint32_t> local_counts(static_cast<std::size_t>(bins), 0);
      #pragma omp for
      for (std::size_t i = 0; i < input.size(); ++i) {
        local_counts[static_cast<std::size_t>(input[i])]++;
      }
      #pragma omp critical
      {
        for (int j = 0; j < bins; ++j) {
          counts[j] += local_counts[j];
        }
      }
    }
  }
  return checksum_u32(counts);
}