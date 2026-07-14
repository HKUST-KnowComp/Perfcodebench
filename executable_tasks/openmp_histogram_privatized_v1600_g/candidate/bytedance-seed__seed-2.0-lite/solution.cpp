#include "interface.h"

#include <algorithm>
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
  const int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  std::vector<std::vector<uint32_t>> private_counts(nthreads, std::vector<uint32_t>(bins));

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      std::memset(private_counts[tid].data(), 0, sizeof(uint32_t) * bins);

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        const std::size_t bin = static_cast<std::size_t>(input[i]);
        private_counts[tid][bin]++;
      }

#pragma omp critical
      {
        for (int b = 0; b < bins; ++b) {
          counts[b] += private_counts[tid][b];
        }
      }
    }
  }
  return checksum_u32(counts);
}