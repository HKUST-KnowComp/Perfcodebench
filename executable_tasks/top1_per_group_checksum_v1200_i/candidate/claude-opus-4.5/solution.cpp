#include "interface.h"

#include <cstring>
#include <algorithm>

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

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;
  const size_t n = groups.size();
  const uint32_t* __restrict g_ptr = groups.data();
  const uint32_t* __restrict v_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(best.data(), 0, group_count * sizeof(uint32_t));
    uint32_t* __restrict b_ptr = best.data();
    
#if defined(__AVX2__)
    size_t i = 0;
    // Process 8 elements at a time using AVX2
    const size_t simd_end = (n >= 7) ? (n - 7) : 0;
    
    for (; i < simd_end; i += 8) {
      __m256i grp = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(g_ptr + i));
      __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v_ptr + i));
      __m256i cur = _mm256_i32gather_epi32(reinterpret_cast<const int*>(b_ptr), grp, 4);
      __m256i mx = _mm256_max_epu32(cur, val);
      
      // Scatter manually - AVX2 doesn't have scatter
      alignas(32) uint32_t grp_arr[8];
      alignas(32) uint32_t mx_arr[8];
      _mm256_store_si256(reinterpret_cast<__m256i*>(grp_arr), grp);
      _mm256_store_si256(reinterpret_cast<__m256i*>(mx_arr), mx);
      
      // Unrolled scatter with conflict handling
      for (int j = 0; j < 8; ++j) {
        uint32_t g = grp_arr[j];
        uint32_t v = mx_arr[j];
        if (v > b_ptr[g]) b_ptr[g] = v;
      }
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > b_ptr[g]) {
        b_ptr[g] = v;
      }
    }
#else
    // Scalar fallback with manual unrolling
    size_t i = 0;
    const size_t unroll_end = (n >= 3) ? (n - 3) : 0;
    
    for (; i < unroll_end; i += 4) {
      uint32_t g0 = g_ptr[i];
      uint32_t v0 = v_ptr[i];
      uint32_t g1 = g_ptr[i + 1];
      uint32_t v1 = v_ptr[i + 1];
      uint32_t g2 = g_ptr[i + 2];
      uint32_t v2 = v_ptr[i + 2];
      uint32_t g3 = g_ptr[i + 3];
      uint32_t v3 = v_ptr[i + 3];
      
      if (v0 > b_ptr[g0]) b_ptr[g0] = v0;
      if (v1 > b_ptr[g1]) b_ptr[g1] = v1;
      if (v2 > b_ptr[g2]) b_ptr[g2] = v2;
      if (v3 > b_ptr[g3]) b_ptr[g3] = v3;
    }
    
    for (; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > b_ptr[g]) {
        b_ptr[g] = v;
      }
    }
#endif
    
    // Compute hash with unrolling
    hash = 1469598103934665603ULL;
    const uint32_t* __restrict bp = best.data();
    
    uint32_t g = 0;
    const uint32_t unroll4_end = (group_count >= 3) ? (group_count - 3) : 0;
    
    for (; g < unroll4_end; g += 4) {
      hash = mix(hash, bp[g]);
      hash = mix(hash, bp[g + 1]);
      hash = mix(hash, bp[g + 2]);
      hash = mix(hash, bp[g + 3]);
    }
    
    for (; g < group_count; ++g) {
      hash = mix(hash, bp[g]);
    }
  }
  
  return hash;
}