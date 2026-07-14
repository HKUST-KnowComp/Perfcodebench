#include "interface.h"
#include <cstdint>
#include <vector>
#include <immintrin.h>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Build direct lookup table -------------------------------------------------
  uint32_t max_key = 0;
  for (uint32_t k : keys) max_key = (k > max_key) ? k : max_key;

  std::vector<uint32_t> lut(max_key + 1, 0);
  for (size_t i = 0; i < keys.size(); ++i) lut[keys[i]] = values[i];

  // Prepare SIMD constants ----------------------------------------------------
  const __m256i mul = _mm256_set1_epi64x(1099511628211ULL);
  const __m256i init = _mm256_set1_epi64x(1469598103934665603ULL);

  const size_t n = probes.size();
  const size_t n_vec = (n / 4) * 4;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    __m256i h4 = init;
    size_t i = 0;

    // Vectorised main body ----------------------------------------------------
    for (; i + 3 < n_vec; i += 4) {
      uint64_t v0 = lut[probes[i + 0]];
      uint64_t v1 = lut[probes[i + 1]];
      uint64_t v2 = lut[probes[i + 2]];
      uint64_t v3 = lut[probes[i + 3]];

      __m256i vals = _mm256_set_epi64x(v3, v2, v1, v0);
      h4 = _mm256_xor_si256(h4, vals);
      h4 = _mm256_mul_epu32(h4, mul);
    }

    // Scalar tail --------------------------------------------------------------
    uint64_t tail_hash = init.m256i_u64[0];
    for (; i < n; ++i) tail_hash = mix(tail_hash, lut[probes[i]]);

    // Combine vector lanes -----------------------------------------------------
    alignas(32) uint64_t lanes[4];
    _mm256_store_si256((__m256i*)lanes, h4);
    uint64_t final_hash = init.m256i_u64[0];
    for (int j = 0; j < 4; ++j) final_hash = mix(final_hash, lanes[j]);

    hash = mix(final_hash, tail_hash);
  }
  return hash;
}