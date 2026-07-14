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

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* base = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = base + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      
      int32_t top1_val, top2_val;
      int top1_idx, top2_idx;
      
      // Find top-2 with tie-breaking: higher value wins, on tie lower index wins
      if (experts >= 8) {
        // Use AVX2 to find max value first
        __m256i max_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row));
        int e = 8;
        for (; e + 7 < experts; e += 8) {
          __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row + e));
          max_vec = _mm256_max_epi32(max_vec, v);
        }
        // Horizontal max
        __m128i lo = _mm256_castsi256_si128(max_vec);
        __m128i hi = _mm256_extracti128_si256(max_vec, 1);
        __m128i max128 = _mm_max_epi32(lo, hi);
        max128 = _mm_max_epi32(max128, _mm_shuffle_epi32(max128, _MM_SHUFFLE(2, 3, 0, 1)));
        max128 = _mm_max_epi32(max128, _mm_shuffle_epi32(max128, _MM_SHUFFLE(1, 0, 3, 2)));
        int32_t simd_max = _mm_cvtsi128_si32(max128);
        
        // Handle tail
        for (; e < experts; ++e) {
          if (row[e] > simd_max) simd_max = row[e];
        }
        
        // Find first index with max value (top1)
        top1_idx = 0;
        for (int i = 0; i < experts; ++i) {
          if (row[i] == simd_max) {
            top1_idx = i;
            break;
          }
        }
        top1_val = simd_max;
        
        // Find top2: best among remaining
        top2_val = INT32_MIN;
        top2_idx = 0;
        for (int i = 0; i < experts; ++i) {
          if (i == top1_idx) continue;
          if (row[i] > top2_val || (row[i] == top2_val && i < top2_idx)) {
            top2_val = row[i];
            top2_idx = i;
          }
        }
      } else {
        // Small expert count: simple scalar
        top1_val = row[0];
        top1_idx = 0;
        for (int i = 1; i < experts; ++i) {
          if (row[i] > top1_val || (row[i] == top1_val && i < top1_idx)) {
            top1_val = row[i];
            top1_idx = i;
          }
        }
        
        top2_val = INT32_MIN;
        top2_idx = 0;
        for (int i = 0; i < experts; ++i) {
          if (i == top1_idx) continue;
          if (row[i] > top2_val || (row[i] == top2_val && i < top2_idx)) {
            top2_val = row[i];
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