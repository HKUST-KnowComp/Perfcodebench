#include "interface.h"

#include <vector>
#include <cstdint>

#ifdef __AVX2__
#include <immintrin.h>
#endif

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const std::size_t n = deltas.size();
  if (n == 0) return 0;
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    uint64_t prefix = base;
    
#ifdef __AVX2__
    const std::size_t simd_end = (n / 4) * 4;
    
    if (simd_end >= 4) {
      __m256i prefix_vec = _mm256_set_epi64x(0, 0, 0, prefix);
      __m256i sum_vec = _mm256_setzero_si256();
      
      for (std::size_t i = 0; i < simd_end; i += 4) {
        __m128i deltas_32 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&deltas[i]));
        __m256i deltas_64 = _mm256_cvtepu32_epi64(deltas_32);
        
        prefix_vec = _mm256_add_epi64(prefix_vec, deltas_64);
        
        __m256i shifted1 = _mm256_bslli_epi128(prefix_vec, 8);
        prefix_vec = _mm256_add_epi64(prefix_vec, shifted1);
        
        __m256i permuted = _mm256_permute4x64_epi64(prefix_vec, 0b10010011);
        __m256i shifted2 = _mm256_blend_epi32(permuted, _mm256_setzero_si256(), 0b00000011);
        prefix_vec = _mm256_add_epi64(prefix_vec, shifted2);
        
        sum_vec = _mm256_add_epi64(sum_vec, prefix_vec);
        
        prefix = _mm256_extract_epi64(prefix_vec, 3);
        prefix_vec = _mm256_set1_epi64x(prefix);
      }
      
      alignas(32) uint64_t sum_arr[4];
      _mm256_store_si256(reinterpret_cast<__m256i*>(sum_arr), sum_vec);
      local_sum = sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3];
    }
    
    for (std::size_t i = simd_end; i < n; ++i) {
      prefix += deltas[i];
      local_sum += prefix;
    }
#else
    const std::size_t unroll_end = (n / 4) * 4;
    
    for (std::size_t i = 0; i < unroll_end; i += 4) {
      prefix += deltas[i];
      local_sum += prefix;
      prefix += deltas[i + 1];
      local_sum += prefix;
      prefix += deltas[i + 2];
      local_sum += prefix;
      prefix += deltas[i + 3];
      local_sum += prefix;
    }
    
    for (std::size_t i = unroll_end; i < n; ++i) {
      prefix += deltas[i];
      local_sum += prefix;
    }
#endif
    
    sum = local_sum;
  }
  
  return sum;
}