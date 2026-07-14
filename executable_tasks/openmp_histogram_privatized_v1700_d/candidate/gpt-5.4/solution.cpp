#include "interface.h"

#include <omp.h>
#include <cstdint>
#include <vector>

namespace {

inline int num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) return 1;
  if (t > 8) return 8;
  return t;
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
    return checksum_u32_ptr(nullptr, 0);
  }

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  const std::size_t n = input.size();

  if (iters <= 0) {
    return checksum_u32_ptr(counts.data(), bins);
  }

  const int threads = num_threads();

  if (bins == 256) {
    const int stride = 256 + 16;
    std::vector<uint32_t> local_storage(static_cast<std::size_t>(threads) * stride, 0u);
    uint32_t* const global_counts = counts.data();
    uint32_t* const locals = local_storage.data();
    const uint8_t* const in = input.data();

    for (int iter = 0; iter < iters; ++iter) {
      for (std::size_t i = 0; i < static_cast<std::size_t>(bins); ++i) global_counts[i] = 0u;

#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* const hist = locals + static_cast<std::size_t>(tid) * stride;

        for (int b = 0; b < 256; ++b) hist[b] = 0u;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++hist[in[i]];
        }

#pragma omp for schedule(static)
        for (int b = 0; b < 256; ++b) {
          uint32_t sum = 0;
          for (int t = 0; t < threads; ++t) {
            sum += locals[static_cast<std::size_t>(t) * stride + b];
          }
          global_counts[b] = sum;
        }
      }
    }

    return checksum_u32_ptr(global_counts, 256);
  }

  for (int iter = 0; iter < iters; ++iter) {
    for (int b = 0; b < bins; ++b) counts[static_cast<std::size_t>(b)] = 0u;

#pragma omp parallel
    {
      std::vector<uint32_t> local(static_cast<std::size_t>(bins), 0u);
      uint32_t* const hist = local.data();
      const uint8_t* const in = input.data();

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        ++hist[static_cast<unsigned>(in[i]) % static_cast<unsigned>(bins)];
      }

#pragma omp critical
      {
        for (int b = 0; b < bins; ++b) {
          counts[static_cast<std::size_t>(b)] += hist[b];
        }
      }
    }
  }

  return checksum_u32_ptr(counts.data(), bins);
}
