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

// Scalar top-2 selection with deterministic tie-breaking (lower index wins)
inline void top2_scalar(const int32_t* row, int experts,
                        int32_t& v1, int& i1, int32_t& v2, int& i2) {
  v1 = row[0];
  i1 = 0;
  v2 = row[1];
  i2 = 1;
  // Ensure v1 >= v2 with tie-break
  if (v2 > v1 || (v2 == v1 && i2 < i1)) {
    int32_t tv = v1; int ti = i1;
    v1 = v2; i1 = i2;
    v2 = tv; i2 = ti;
  }
  for (int e = 2; e < experts; ++e) {
    int32_t val = row[e];
    if (val > v1 || (val == v1 && e < i1)) {
      v2 = v1; i2 = i1;
      v1 = val; i1 = e;
    } else if (val > v2 || (val == v2 && e < i2)) {
      v2 = val; i2 = e;
    }
  }
}

#if defined(__AVX2__)
// AVX2 top-2 selection
inline void top2_avx2(const int32_t* row, int experts,
                      int32_t& v1, int& i1, int32_t& v2, int& i2) {
  // Process 8 elements at a time
  __m256i max1_val = _mm256_set1_epi32(INT32_MIN);
  __m256i max1_idx = _mm256_setzero_si256();
  __m256i max2_val = _mm256_set1_epi32(INT32_MIN);
  __m256i max2_idx = _mm256_setzero_si256();
  __m256i idx_base = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
  __m256i eight = _mm256_set1_epi32(8);

  int e = 0;
  for (; e + 8 <= experts; e += 8) {
    __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row + e));
    __m256i idxs = idx_base;

    // Check if vals > max1_val, or (vals == max1_val && idxs < max1_idx)
    __m256i gt1 = _mm256_cmpgt_epi32(vals, max1_val);
    __m256i eq1 = _mm256_cmpeq_epi32(vals, max1_val);
    __m256i lt_idx1 = _mm256_cmpgt_epi32(max1_idx, idxs);
    __m256i better1 = _mm256_or_si256(gt1, _mm256_and_si256(eq1, lt_idx1));

    // For elements that beat max1: old max1 becomes candidate for max2
    // Update max2 first with old max1 values where better1 is true
    __m256i gt2_from_old = _mm256_cmpgt_epi32(max1_val, max2_val);
    __m256i eq2_from_old = _mm256_cmpeq_epi32(max1_val, max2_val);
    __m256i lt_idx2_from_old = _mm256_cmpgt_epi32(max2_idx, max1_idx);
    __m256i better2_from_old = _mm256_or_si256(gt2_from_old, _mm256_and_si256(eq2_from_old, lt_idx2_from_old));
    __m256i update2_from_old = _mm256_and_si256(better1, better2_from_old);
    max2_val = _mm256_blendv_epi8(max2_val, max1_val, update2_from_old);
    max2_idx = _mm256_blendv_epi8(max2_idx, max1_idx, update2_from_old);

    // Update max1
    max1_val = _mm256_blendv_epi8(max1_val, vals, better1);
    max1_idx = _mm256_blendv_epi8(max1_idx, idxs, better1);

    // For elements that don't beat max1, check if they beat max2
    __m256i not_better1 = _mm256_xor_si256(better1, _mm256_set1_epi32(-1));
    __m256i gt2 = _mm256_cmpgt_epi32(vals, max2_val);
    __m256i eq2 = _mm256_cmpeq_epi32(vals, max2_val);
    __m256i lt_idx2 = _mm256_cmpgt_epi32(max2_idx, idxs);
    __m256i better2 = _mm256_and_si256(not_better1, _mm256_or_si256(gt2, _mm256_and_si256(eq2, lt_idx2)));
    max2_val = _mm256_blendv_epi8(max2_val, vals, better2);
    max2_idx = _mm256_blendv_epi8(max2_idx, idxs, better2);

    idx_base = _mm256_add_epi32(idx_base, eight);
  }

  // Horizontal reduction
  alignas(32) int32_t vals1[8], idxs1[8], vals2[8], idxs2[8];
  _mm256_store_si256(reinterpret_cast<__m256i*>(vals1), max1_val);
  _mm256_store_si256(reinterpret_cast<__m256i*>(idxs1), max1_idx);
  _mm256_store_si256(reinterpret_cast<__m256i*>(vals2), max2_val);
  _mm256_store_si256(reinterpret_cast<__m256i*>(idxs2), max2_idx);

  v1 = vals1[0]; i1 = idxs1[0];
  v2 = vals2[0]; i2 = idxs2[0];
  if (v2 > v1 || (v2 == v1 && i2 < i1)) {
    int32_t tv = v1; int ti = i1;
    v1 = v2; i1 = i2;
    v2 = tv; i2 = ti;
  }

  for (int j = 1; j < 8; ++j) {
    int32_t cv1 = vals1[j], cv2 = vals2[j];
    int ci1 = idxs1[j], ci2 = idxs2[j];
    if (cv2 > cv1 || (cv2 == cv1 && ci2 < ci1)) {
      int32_t tv = cv1; int ti = ci1;
      cv1 = cv2; ci1 = ci2;
      cv2 = tv; ci2 = ti;
    }
    // Merge (cv1, ci1), (cv2, ci2) into (v1, i1), (v2, i2)
    if (cv1 > v1 || (cv1 == v1 && ci1 < i1)) {
      // cv1 is new top1
      // new top2 is max of old v1 and cv2
      if (v1 > cv2 || (v1 == cv2 && i1 < ci2)) {
        v2 = v1; i2 = i1;
      } else {
        v2 = cv2; i2 = ci2;
      }
      v1 = cv1; i1 = ci1;
    } else {
      // v1 stays top1, check cv1 against v2
      if (cv1 > v2 || (cv1 == v2 && ci1 < i2)) {
        v2 = cv1; i2 = ci1;
      }
    }
  }

  // Handle tail
  for (; e < experts; ++e) {
    int32_t val = row[e];
    if (val > v1 || (val == v1 && e < i1)) {
      v2 = v1; i2 = i1;
      v1 = val; i1 = e;
    } else if (val > v2 || (val == v2 && e < i2)) {
      v2 = val; i2 = e;
    }
  }
}
#endif

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* data = logits.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      int32_t v1, v2;
      int i1, i2;

#if defined(__AVX2__)
      if (experts >= 16) {
        top2_avx2(row, experts, v1, i1, v2, i2);
      } else {
        top2_scalar(row, experts, v1, i1, v2, i2);
      }
#else
      top2_scalar(row, experts, v1, i1, v2, i2);
#endif

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v1)));
      hash = mix(hash, static_cast<uint64_t>(i1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v2)));
      hash = mix(hash, static_cast<uint64_t>(i2));
    }
  }
  return hash;
}