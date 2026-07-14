#include "interface.h"

#include <omp.h>
#include <cstdint>
#include <vector>

namespace {

static inline int tuned_num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) t = 1;
  if (t > 8) t = 8;
  return t;
}

static inline uint64_t checksum_u32_ptr(const uint32_t* data, int n) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  if (bins <= 0) {
    return 1469598103934665603ULL;
  }

  const int threads = tuned_num_threads();
  omp_set_num_threads(threads);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  if (iters <= 0) {
    return checksum_u32_ptr(counts.data(), bins);
  }

  const std::size_t n = input.size();
  const uint8_t* in = input.data();

  // Pad each thread's histogram to reduce false sharing and improve alignment.
  const int stride = ((bins + 15) / 16) * 16;
  std::vector<uint32_t> local_all(static_cast<std::size_t>(threads) * static_cast<std::size_t>(stride), 0u);

  for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local = local_all.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(stride);

      for (int b = 0; b < bins; ++b) local[b] = 0u;

      if (bins == 256) {
#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++local[in[i]];
        }
      } else {
#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          const unsigned v = static_cast<unsigned>(in[i]);
          if (v < static_cast<unsigned>(bins)) {
            ++local[v];
          }
        }
      }

#pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < threads; ++t) {
          sum += local_all[static_cast<std::size_t>(t) * static_cast<std::size_t>(stride) + static_cast<std::size_t>(b)];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }
  }

  return checksum_u32_ptr(counts.data(), bins);
}
