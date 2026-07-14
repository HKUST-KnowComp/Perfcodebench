#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstdint>

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
      // Thread-private stack-allocated histogram to avoid atomic contention
      uint32_t private_counts[256] = {0};
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        private_counts[static_cast<std::size_t>(input[i])]++;
      }
      // Merge private counts into global with minimal synchronization work
      #pragma omp critical
      {
        for (int b = 0; b < bins; ++b) {
          counts[b] += private_counts[b];
        }
      }
    }
  }
  return checksum_u32(counts);
}