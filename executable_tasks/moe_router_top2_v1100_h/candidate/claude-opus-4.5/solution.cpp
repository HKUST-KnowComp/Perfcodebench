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

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* base = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = base + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      
      int32_t top1_val, top2_val;
      int top1_idx, top2_idx;
      
#if defined(__AVX2__)
      if (experts >= 8) {
        // First pass: find max value using SIMD
        __m256i vmax = _mm256_set1_epi32(INT32_MIN);
        int e = 0;
        for (; e + 8 <= experts; e += 8) {
          __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row + e));
          vmax = _mm256_max_epi32(vmax, v);
        }
        // Horizontal max
        __m128i lo = _mm256_castsi256_si128(vmax);
        __m128i hi = _mm256_extracti128_si256(vmax, 1);
        __m128i m128 = _mm_max_epi32(lo, hi);
        m128 = _mm_max_epi32(m128, _mm_shuffle_epi32(m128, _MM_SHUFFLE(2, 3, 0, 1)));
        m128 = _mm_max_epi32(m128, _mm_shuffle_epi32(m128, _MM_SHUFFLE(1, 0, 3, 2)));
        int32_t max_val = _mm_cvtsi128_si32(m128);
        // Handle tail
        for (; e < experts; ++e) {
          if (row[e] > max_val) max_val = row[e];
        }
        
        // Second pass: find top1 (first occurrence of max) and top2
        top1_val = INT32_MIN;
        top1_idx = 0;
        top2_val = INT32_MIN;
        top2_idx = 0;
        
        for (int i = 0; i < experts; ++i) {
          int32_t v = row[i];
          if (v > top1_val || (v == top1_val && i < top1_idx)) {
            top2_val = top1_val;
            top2_idx = top1_idx;
            top1_val = v;
            top1_idx = i;
          } else if (v > top2_val || (v == top2_val && i < top2_idx)) {
            top2_val = v;
            top2_idx = i;
          }
        }
      } else
#endif
      {
        // Scalar path for small expert counts or no AVX2
        top1_val = INT32_MIN;
        top1_idx = 0;
        top2_val = INT32_MIN;
        top2_idx = 0;
        
        for (int i = 0; i < experts; ++i) {
          int32_t v = row[i];
          if (v > top1_val || (v == top1_val && i < top1_idx)) {
            top2_val = top1_val;
            top2_idx = top1_idx;
            top1_val = v;
            top1_idx = i;
          } else if (v > top2_val || (v == top2_val && i < top2_idx)) {
            top2_val = v;
            top2_idx = i;
          }
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
  }
  return hash;
}