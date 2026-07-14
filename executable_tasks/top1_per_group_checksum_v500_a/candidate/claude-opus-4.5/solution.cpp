#include "interface.h"

#include <cstring>
#include <immintrin.h>

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
    
    size_t i = 0;
    
#if defined(__AVX2__)
    // Process 8 elements at a time using AVX2 gather
    const size_t simd_end = (n / 8) * 8;
    for (; i < simd_end; i += 8) {
      __m256i grp = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(g_ptr + i));
      __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v_ptr + i));
      __m256i cur = _mm256_i32gather_epi32(reinterpret_cast<const int*>(b_ptr), grp, 4);
      __m256i new_max = _mm256_max_epu32(cur, val);
      
      // Check if any updates needed
      __m256i changed = _mm256_xor_si256(cur, new_max);
      if (!_mm256_testz_si256(changed, changed)) {
        // Scalar fallback for scatter (AVX2 doesn't have scatter)
        alignas(32) uint32_t grp_arr[8];
        alignas(32) uint32_t max_arr[8];
        _mm256_store_si256(reinterpret_cast<__m256i*>(grp_arr), grp);
        _mm256_store_si256(reinterpret_cast<__m256i*>(max_arr), new_max);
        
        for (int j = 0; j < 8; ++j) {
          uint32_t g = grp_arr[j];
          uint32_t v = max_arr[j];
          if (v > b_ptr[g]) b_ptr[g] = v;
        }
      }
    }
#endif
    
    // Scalar remainder
    for (; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > b_ptr[g]) {
        b_ptr[g] = v;
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
  }
  return hash;
}