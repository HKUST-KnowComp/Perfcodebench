#include "interface.h"

#include <cstdint>
#include <omp.h>
#include <vector>

namespace {

inline int num_threads() {
  int mt = omp_get_max_threads();
  if (mt < 1) return 1;
  return mt > 8 ? 8 : mt;
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

  const int threads = num_threads();
  const std::size_t n = input.size();
  const uint8_t* in = input.data();

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);

  if (iters <= 0) {
    return checksum_u32_ptr(counts.data(), bins);
  }

  if (bins == 256) {
    const int stride = 256;
    std::vector<uint32_t> local_storage(static_cast<std::size_t>(threads) * stride, 0u);
    uint32_t* locals = local_storage.data();

    for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* hist = locals + static_cast<std::size_t>(tid) * stride;

        for (int b = 0; b < 256; ++b) hist[b] = 0u;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++hist[in[i]];
        }
      }

      for (int b = 0; b < 256; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < threads; ++t) {
          sum += locals[static_cast<std::size_t>(t) * stride + b];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }

    return checksum_u32_ptr(counts.data(), 256);
  }

  std::vector<uint32_t> local_storage(static_cast<std::size_t>(threads) * static_cast<std::size_t>(bins), 0u);
  uint32_t* locals = local_storage.data();

  for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* hist = locals + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

      for (int b = 0; b < bins; ++b) hist[b] = 0u;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        const std::size_t idx = static_cast<std::size_t>(in[i]);
        if (idx < static_cast<std::size_t>(bins)) {
          ++hist[idx];
        }
      }
    }

    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0u;
      for (int t = 0; t < threads; ++t) {
        sum += locals[static_cast<std::size_t>(t) * static_cast<std::size_t>(bins) + static_cast<std::size_t>(b)];
      }
      counts[static_cast<std::size_t>(b)] = sum;
    }
  }

  return checksum_u32_ptr(counts.data(), bins);
}
