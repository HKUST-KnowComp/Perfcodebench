#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t fnv1a_u32_seq(const uint32_t* data, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

static inline uint64_t pow_fnv_prime(std::size_t exp) {
  uint64_t base = 1099511628211ULL;
  uint64_t result = 1ULL;
  while (exp) {
    if (exp & 1U) result *= base;
    base *= base;
    exp >>= 1U;
  }
  return result;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = input.size();
  if (total == 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint32_t> a = input;
  if (steps <= 0) {
    return fnv1a_u32_seq(a.data(), total);
  }

  std::vector<uint32_t> b(total);

  const int r = rows;
  const int c = cols;

  if (r <= 0 || c <= 0) {
    return fnv1a_u32_seq(a.data(), total);
  }

  if (r < 3 || c < 3) {
    return fnv1a_u32_seq(a.data(), total);
  }

  for (int step = 0; step < steps; ++step) {
    const uint32_t* __restrict src = a.data();
    uint32_t* __restrict dst = b.data();

    // Copy top and bottom rows unchanged.
    #pragma omp parallel for schedule(static)
    for (int j = 0; j < c; ++j) {
      dst[j] = src[j];
      dst[static_cast<std::size_t>(r - 1) * static_cast<std::size_t>(c) + static_cast<std::size_t>(j)] =
          src[static_cast<std::size_t>(r - 1) * static_cast<std::size_t>(c) + static_cast<std::size_t>(j)];
    }

    // Process interior rows; copy left/right borders and compute interior without branches.
    #pragma omp parallel for schedule(static)
    for (int i = 1; i < r - 1; ++i) {
      const std::size_t base = static_cast<std::size_t>(i) * static_cast<std::size_t>(c);
      const std::size_t up = base - static_cast<std::size_t>(c);
      const std::size_t down = base + static_cast<std::size_t>(c);

      dst[base] = src[base];
      dst[base + static_cast<std::size_t>(c - 1)] = src[base + static_cast<std::size_t>(c - 1)];

      for (int j = 1; j < c - 1; ++j) {
        const std::size_t idx = base + static_cast<std::size_t>(j);
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + src[up + static_cast<std::size_t>(j)] +
                    src[down + static_cast<std::size_t>(j)]) /
                   8u;
      }
    }

    a.swap(b);
  }

  const uint32_t* data = a.data();

  int max_threads = omp_get_max_threads();
  if (max_threads < 1) max_threads = 1;
  if (total < static_cast<std::size_t>(max_threads) * 1024ULL) {
    return fnv1a_u32_seq(data, total);
  }

  std::vector<uint64_t> chunk_hash(static_cast<std::size_t>(max_threads));
  std::vector<std::size_t> chunk_len(static_cast<std::size_t>(max_threads));

  #pragma omp parallel
  {
    const int tid = omp_get_thread_num();
    const int nth = omp_get_num_threads();
    const std::size_t t = static_cast<std::size_t>(tid);
    const std::size_t nthreads = static_cast<std::size_t>(nth);

    const std::size_t begin = (total * t) / nthreads;
    const std::size_t end = (total * (t + 1)) / nthreads;

    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = begin; i < end; ++i) {
      h ^= static_cast<uint64_t>(data[i]);
      h *= 1099511628211ULL;
    }
    chunk_hash[t] = h;
    chunk_len[t] = end - begin;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int t = 0; t < max_threads; ++t) {
    const std::size_t len = chunk_len[static_cast<std::size_t>(t)];
    if (len == 0) continue;
    const uint64_t h = chunk_hash[static_cast<std::size_t>(t)];
    hash = h ^ (hash * pow_fnv_prime(len));
  }
  return hash;
}
