#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

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
  const int n_threads = num_threads();
  omp_set_num_threads(n_threads);
  
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  const std::size_t input_size = input.size();
  const uint8_t* data_ptr = input.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);

    #pragma omp parallel
    {
      // Privatize the histogram to avoid atomic contention
      // Using a fixed size array on stack for speed (bins is 256)
      uint32_t local_counts[256] = {0};

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input_size; ++i) {
        local_counts[data_ptr[i]]++;
      }

      // Merge local results into global counts
      // We use atomic here, but it only happens once per thread per iteration
      for (int b = 0; b < bins; ++b) {
        if (local_counts[b] > 0) {
          #pragma omp atomic
          global_counts[b] += local_counts[b];
        }
      }
    }
  }

  return checksum_u32(global_counts);
}
