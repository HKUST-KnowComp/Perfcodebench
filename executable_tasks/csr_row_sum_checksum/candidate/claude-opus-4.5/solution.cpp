#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

inline uint64_t row_sum(const uint32_t* __restrict__ data, std::size_t len) {
  uint64_t sum = 0;
#if defined(__AVX2__)
  if (len >= 8) {
    __m256i vsum = _mm256_setzero_si256();
    std::size_t i = 0;
    std::size_t vec_end = len - (len & 7);
    for (; i < vec_end; i += 8) {
      __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      // Zero-extend 32-bit to 64-bit and accumulate
      __m256i lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(v));
      __m256i hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(v, 1));
      vsum = _mm256_add_epi64(vsum, lo);
      vsum = _mm256_add_epi64(vsum, hi);
    }
    // Horizontal sum of 4 x 64-bit
    __m128i lo128 = _mm256_castsi256_si128(vsum);
    __m128i hi128 = _mm256_extracti128_si256(vsum, 1);
    __m128i sum128 = _mm_add_epi64(lo128, hi128);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) +
          static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    for (; i < len; ++i) {
      sum += data[i];
    }
  } else {
    for (std::size_t i = 0; i < len; ++i) {
      sum += data[i];
    }
  }
#else
  for (std::size_t i = 0; i < len; ++i) {
    sum += data[i];
  }
#endif
  return sum;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const std::size_t num_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  const uint32_t* __restrict__ rp = row_ptr.data();
  const uint32_t* __restrict__ vals = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_rows; ++r) {
      std::size_t start = rp[r];
      std::size_t end = rp[r + 1];
      uint64_t sum = row_sum(vals + start, end - start);
      hash = mix(hash, sum);
    }
  }
  return hash;
}