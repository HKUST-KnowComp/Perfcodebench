#include "interface.h"

#include <cstdint>
#include <immintrin.h>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Find top-2 with deterministic tie-breaking (lower index wins)
inline void find_top2(const int32_t* row, int experts,
                      int32_t& val1, int& idx1,
                      int32_t& val2, int& idx2) {
  // First pass: find maximum value
  int32_t max_val = row[0];
  int experts8 = experts & ~7;
  
  if (experts8 >= 8) {
    __m256i vmax = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row));
    for (int e = 8; e < experts8; e += 8) {
      __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row + e));
      vmax = _mm256_max_epi32(vmax, v);
    }
    // Horizontal max
    __m128i lo = _mm256_castsi256_si128(vmax);
    __m128i hi = _mm256_extracti128_si256(vmax, 1);
    __m128i m = _mm_max_epi32(lo, hi);
    m = _mm_max_epi32(m, _mm_shuffle_epi32(m, _MM_SHUFFLE(1,0,3,2)));
    m = _mm_max_epi32(m, _mm_shuffle_epi32(m, _MM_SHUFFLE(2,3,0,1)));
    max_val = _mm_cvtsi128_si32(m);
  }
  for (int e = experts8; e < experts; ++e) {
    if (row[e] > max_val) max_val = row[e];
  }
  
  // Find first index with max_val (smallest index for tie-breaking)
  idx1 = 0;
  if (experts8 >= 8) {
    __m256i vtarget = _mm256_set1_epi32(max_val);
    for (int e = 0; e < experts8; e += 8) {
      __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row + e));
      __m256i cmp = _mm256_cmpeq_epi32(v, vtarget);
      int mask = _mm256_movemask_ps(_mm256_castsi256_ps(cmp));
      if (mask) {
        idx1 = e + __builtin_ctz(static_cast<unsigned>(mask));
        goto found_first;
      }
    }
  }
  for (int e = (experts8 >= 8 ? experts8 : 0); e < experts; ++e) {
    if (row[e] == max_val) {
      idx1 = e;
      goto found_first;
    }
  }
found_first:
  val1 = max_val;
  
  // Second pass: find second maximum (excluding idx1)
  int32_t max2_val;
  if (idx1 == 0) {
    max2_val = (experts > 1) ? row[1] : row[0];
  } else {
    max2_val = row[0];
  }
  
  for (int e = 0; e < experts; ++e) {
    if (e == idx1) continue;
    if (row[e] > max2_val) max2_val = row[e];
  }
  
  // Find first index with max2_val (excluding idx1)
  idx2 = -1;
  for (int e = 0; e < experts; ++e) {
    if (e == idx1) continue;
    if (row[e] == max2_val) {
      idx2 = e;
      break;
    }
  }
  val2 = max2_val;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* data = logits.data();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      int32_t val1, val2;
      int idx1, idx2;
      find_top2(row, experts, val1, idx1, val2, idx2);
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(val1)));
      hash = mix(hash, static_cast<uint64_t>(idx1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(val2)));
      hash = mix(hash, static_cast<uint64_t>(idx2));
    }
  }
  return hash;
}
