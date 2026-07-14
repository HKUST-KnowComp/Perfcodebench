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
  std::vector<uint32_t> global_counts(256, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);

    #pragma omp parallel
    {
      uint32_t local_counts[256] = {0};
      
      #pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_counts[data[i]]++;
      }

      #pragma omp critical
      {
        for (int i = 0; i < 256; ++i) {
          global_counts[i] += local_counts[i];
        }
      }
    }
  }
  return checksum_u32(global_counts);
}