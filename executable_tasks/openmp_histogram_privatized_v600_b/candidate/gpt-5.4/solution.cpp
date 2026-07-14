#include "interface.h"

#include <omp.h>
#include <cstdint>
#include <vector>

namespace {

inline int num_threads() {
  int mt = omp_get_max_threads();
  if (mt < 1) return 1;
  return mt < 8 ? mt : 8;
}

inline uint64_t checksum_u32_ptr(const uint32_t* data, int n) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  omp_set_num_threads(num_threads());

  if (bins <= 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins));
  const std::size_t n = input.size();
  const uint8_t* in = input.data();

  // Fast path for the task's intended configuration: 256 bins over uint8_t input.
  if (bins == 256) {
    const int max_threads = num_threads();
    const int stride = 256 + 16;  // padding to reduce false sharing
    std::vector<uint32_t> local_storage(static_cast<std::size_t>(max_threads) * stride);

    for (int iter = 0; iter < iters; ++iter) {
      uint32_t* global = counts.data();
      for (int b = 0; b < 256; ++b) global[b] = 0;

#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* local = local_storage.data() + static_cast<std::size_t>(tid) * stride;

        for (int b = 0; b < 256; ++b) local[b] = 0;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++local[in[i]];
        }

#pragma omp for schedule(static)
        for (int b = 0; b < 256; ++b) {
          uint32_t sum = 0;
          for (int t = 0; t < max_threads; ++t) {
            sum += local_storage[static_cast<std::size_t>(t) * stride + b];
          }
          global[b] = sum;
        }
      }
    }

    return checksum_u32_ptr(counts.data(), 256);
  }

  // Generic fallback for arbitrary bin counts.
  const int max_threads = num_threads();
  const std::size_t bins_sz = static_cast<std::size_t>(bins);
  std::vector<uint32_t> local_storage(static_cast<std::size_t>(max_threads) * bins_sz);

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* global = counts.data();
    for (int b = 0; b < bins; ++b) global[b] = 0;

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local = local_storage.data() + static_cast<std::size_t>(tid) * bins_sz;

      for (int b = 0; b < bins; ++b) local[b] = 0;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        ++local[static_cast<std::size_t>(in[i])];
      }

#pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0;
        for (int t = 0; t < max_threads; ++t) {
          sum += local_storage[static_cast<std::size_t>(t) * bins_sz + static_cast<std::size_t>(b)];
        }
        global[b] = sum;
      }
    }
  }

  return checksum_u32_ptr(counts.data(), bins);
}
