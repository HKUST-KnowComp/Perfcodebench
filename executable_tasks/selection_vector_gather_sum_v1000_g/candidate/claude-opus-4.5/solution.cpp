#include "interface.h"

#include <cstdint>
#include <cstring>
#include <immintrin.h>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {

  const std::size_t n = keys.size();
  if (n == 0 || iters <= 0) return 0;

  const uint32_t* __restrict kptr = keys.data();
  const uint8_t*  __restrict fptr = flags.data();
  const uint32_t* __restrict pptr = payloads.data();

  // Pre-allocate selection buffer
  uint32_t* selected = static_cast<uint32_t*>(std::aligned_alloc(64, (n + 32) * sizeof(uint32_t)));
  if (!selected) {
    // Fallback to scalar
    std::vector<uint32_t> sel;
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      sel.clear();
      for (std::size_t i = 0; i < n; ++i) {
        if (fptr[i] && kptr[i] < threshold && ((kptr[i] & 7u) == 0u)) {
          sel.push_back(static_cast<uint32_t>(i));
        }
      }
      sum = 0;
      for (uint32_t idx : sel) {
        sum += static_cast<uint64_t>(pptr[idx]);
      }
    }
    return sum;
  }

  uint64_t sum = 0;

  // AVX2 constants
  const __m256i thresh_vec = _mm256_set1_epi32(static_cast<int>(threshold));
  const __m256i mask7 = _mm256_set1_epi32(7);
  const __m256i zero = _mm256_setzero_si256();

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t sel_count = 0;

    std::size_t i = 0;

    // Process 8 elements at a time with AVX2
    const std::size_t vec_end = (n >= 8) ? (n - 7) : 0;
    
    while (i < vec_end) {
      // Load 8 keys
      __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(kptr + i));

      // Load 8 flags (as bytes, then expand)
      uint64_t fbytes;
      std::memcpy(&fbytes, fptr + i, 8);
      __m128i f8 = _mm_cvtsi64_si128(static_cast<long long>(fbytes));
      __m256i f32 = _mm256_cvtepu8_epi32(f8);

      // flags != 0
      __m256i cmp_flags = _mm256_cmpgt_epi32(f32, zero);

      // keys < threshold (unsigned compare via offset trick)
      // For unsigned: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) signed
      const __m256i sign_bit = _mm256_set1_epi32(static_cast<int>(0x80000000u));
      __m256i k_signed = _mm256_xor_si256(k, sign_bit);
      __m256i t_signed = _mm256_xor_si256(thresh_vec, sign_bit);
      __m256i cmp_thresh = _mm256_cmpgt_epi32(t_signed, k_signed);

      // (keys & 7) == 0
      __m256i k_and_7 = _mm256_and_si256(k, mask7);
      __m256i cmp_mod = _mm256_cmpeq_epi32(k_and_7, zero);

      // Combine all conditions
      __m256i cmp_all = _mm256_and_si256(cmp_flags, _mm256_and_si256(cmp_thresh, cmp_mod));

      // Extract mask
      int mask = _mm256_movemask_ps(_mm256_castsi256_ps(cmp_all));

      // Store selected indices
      while (mask) {
        int bit = __builtin_ctz(mask);
        selected[sel_count++] = static_cast<uint32_t>(i + bit);
        mask &= mask - 1;
      }

      i += 8;
    }

    // Scalar tail
    for (; i < n; ++i) {
      if (fptr[i] && kptr[i] < threshold && ((kptr[i] & 7u) == 0u)) {
        selected[sel_count++] = static_cast<uint32_t>(i);
      }
    }

    // Gather sum with unrolling
    sum = 0;
    std::size_t j = 0;
    const std::size_t sel_end4 = (sel_count >= 4) ? (sel_count - 3) : 0;
    
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (; j < sel_end4; j += 4) {
      s0 += pptr[selected[j]];
      s1 += pptr[selected[j + 1]];
      s2 += pptr[selected[j + 2]];
      s3 += pptr[selected[j + 3]];
    }
    sum = s0 + s1 + s2 + s3;
    
    for (; j < sel_count; ++j) {
      sum += pptr[selected[j]];
    }
  }

  std::free(selected);
  return sum;
}
