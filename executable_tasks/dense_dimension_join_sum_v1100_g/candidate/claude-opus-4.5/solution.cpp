#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <emmintrin.h>  // SSE2

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  // Build direct-lookup table: lookup[key] = value
  // Find min and max keys to size the table
  if (dim_keys.empty()) {
    return 0;
  }

  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_keys.size(); ++i) {
    if (dim_keys[i] < min_key) min_key = dim_keys[i];
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  const std::size_t table_size = static_cast<std::size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> lookup(table_size, 0);

  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  const uint32_t* lookup_ptr = lookup.data();
  const uint32_t* probe_keys_ptr = probe_keys.data();
  const uint32_t* probe_weights_ptr = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    __m128i acc0 = _mm_setzero_si128();
    __m128i acc1 = _mm_setzero_si128();
    __m128i acc2 = _mm_setzero_si128();
    __m128i acc3 = _mm_setzero_si128();

    std::size_t i = 0;

    // Process 8 elements at a time with 4 accumulators
    const std::size_t unroll_limit = (n >= 7) ? (n - 7) : 0;
    for (; i < unroll_limit; i += 8) {
      // Load 8 keys and 8 weights
      uint32_t k0 = probe_keys_ptr[i];
      uint32_t k1 = probe_keys_ptr[i + 1];
      uint32_t k2 = probe_keys_ptr[i + 2];
      uint32_t k3 = probe_keys_ptr[i + 3];
      uint32_t k4 = probe_keys_ptr[i + 4];
      uint32_t k5 = probe_keys_ptr[i + 5];
      uint32_t k6 = probe_keys_ptr[i + 6];
      uint32_t k7 = probe_keys_ptr[i + 7];

      uint32_t v0 = lookup_ptr[k0 - min_key];
      uint32_t v1 = lookup_ptr[k1 - min_key];
      uint32_t v2 = lookup_ptr[k2 - min_key];
      uint32_t v3 = lookup_ptr[k3 - min_key];
      uint32_t v4 = lookup_ptr[k4 - min_key];
      uint32_t v5 = lookup_ptr[k5 - min_key];
      uint32_t v6 = lookup_ptr[k6 - min_key];
      uint32_t v7 = lookup_ptr[k7 - min_key];

      uint32_t w0 = probe_weights_ptr[i];
      uint32_t w1 = probe_weights_ptr[i + 1];
      uint32_t w2 = probe_weights_ptr[i + 2];
      uint32_t w3 = probe_weights_ptr[i + 3];
      uint32_t w4 = probe_weights_ptr[i + 4];
      uint32_t w5 = probe_weights_ptr[i + 5];
      uint32_t w6 = probe_weights_ptr[i + 6];
      uint32_t w7 = probe_weights_ptr[i + 7];

      // Compute 64-bit products and accumulate
      __m128i prod0 = _mm_set_epi64x(
          static_cast<int64_t>(static_cast<uint64_t>(v1) * w1),
          static_cast<int64_t>(static_cast<uint64_t>(v0) * w0));
      __m128i prod1 = _mm_set_epi64x(
          static_cast<int64_t>(static_cast<uint64_t>(v3) * w3),
          static_cast<int64_t>(static_cast<uint64_t>(v2) * w2));
      __m128i prod2 = _mm_set_epi64x(
          static_cast<int64_t>(static_cast<uint64_t>(v5) * w5),
          static_cast<int64_t>(static_cast<uint64_t>(v4) * w4));
      __m128i prod3 = _mm_set_epi64x(
          static_cast<int64_t>(static_cast<uint64_t>(v7) * w7),
          static_cast<int64_t>(static_cast<uint64_t>(v6) * w6));

      acc0 = _mm_add_epi64(acc0, prod0);
      acc1 = _mm_add_epi64(acc1, prod1);
      acc2 = _mm_add_epi64(acc2, prod2);
      acc3 = _mm_add_epi64(acc3, prod3);
    }

    // Combine accumulators
    __m128i acc01 = _mm_add_epi64(acc0, acc1);
    __m128i acc23 = _mm_add_epi64(acc2, acc3);
    __m128i acc_all = _mm_add_epi64(acc01, acc23);

    // Extract and sum the two 64-bit values
    uint64_t tmp[2];
    _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), acc_all);
    sum = tmp[0] + tmp[1];

    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = probe_keys_ptr[i];
      uint32_t v = lookup_ptr[k - min_key];
      uint32_t w = probe_weights_ptr[i];
      sum += static_cast<uint64_t>(v) * static_cast<uint64_t>(w);
    }
  }

  return sum;
}