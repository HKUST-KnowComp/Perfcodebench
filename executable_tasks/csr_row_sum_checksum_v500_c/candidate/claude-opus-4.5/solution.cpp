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

inline uint64_t sum_row(const uint32_t* __restrict__ data, std::size_t len) {
  uint64_t sum = 0;
  std::size_t i = 0;

#if defined(__AVX2__)
  if (len >= 8) {
    __m256i acc_lo = _mm256_setzero_si256();
    __m256i acc_hi = _mm256_setzero_si256();
    
    for (; i + 7 < len; i += 8) {
      __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      // Unpack to 64-bit to avoid overflow
      __m256i lo = _mm256_unpacklo_epi32(v, _mm256_setzero_si256());
      __m256i hi = _mm256_unpackhi_epi32(v, _mm256_setzero_si256());
      acc_lo = _mm256_add_epi64(acc_lo, lo);
      acc_hi = _mm256_add_epi64(acc_hi, hi);
    }
    
    __m256i acc = _mm256_add_epi64(acc_lo, acc_hi);
    // Horizontal sum of 4 x 64-bit values
    __m128i lo128 = _mm256_castsi256_si128(acc);
    __m128i hi128 = _mm256_extracti128_si256(acc, 1);
    __m128i sum128 = _mm_add_epi64(lo128, hi128);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
  }
#endif

  // Scalar tail
  for (; i < len; ++i) {
    sum += static_cast<uint64_t>(data[i]);
  }
  
  return sum;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  const std::size_t num_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  const uint32_t* __restrict__ rp = row_ptr.data();
  const uint32_t* __restrict__ vals = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t r = 0; r < num_rows; ++r) {
      const std::size_t start = static_cast<std::size_t>(rp[r]);
      const std::size_t end = static_cast<std::size_t>(rp[r + 1]);
      const std::size_t len = end - start;
      
      uint64_t sum = sum_row(vals + start, len);
      hash = mix(hash, sum);
    }
  }
  
  return hash;
}