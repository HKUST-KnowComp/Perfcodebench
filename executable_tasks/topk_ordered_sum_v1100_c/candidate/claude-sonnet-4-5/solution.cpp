#include "interface.h"

#include <algorithm>
#include <immintrin.h>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp;
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp = scores;
    
    // Partition so that the k-th element is in its sorted position
    // and all elements before it are >= it
    std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
    
    // Sum the top k elements using SIMD
    sum = 0;
    int i = 0;
    
    // Process 8 elements at a time with AVX2
    __m256i sum_vec = _mm256_setzero_si256();
    for (; i + 7 < k; i += 8) {
      __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&tmp[i]));
      // Convert uint32 to uint64 for accumulation without overflow
      __m128i low = _mm256_castsi256_si128(vals);
      __m128i high = _mm256_extracti128_si256(vals, 1);
      __m256i low64 = _mm256_cvtepu32_epi64(low);
      __m256i high64 = _mm256_cvtepu32_epi64(high);
      sum_vec = _mm256_add_epi64(sum_vec, low64);
      sum_vec = _mm256_add_epi64(sum_vec, high64);
    }
    
    // Horizontal sum of the vector
    alignas(32) uint64_t temp[4];
    _mm256_store_si256(reinterpret_cast<__m256i*>(temp), sum_vec);
    sum = temp[0] + temp[1] + temp[2] + temp[3];
    
    // Handle remaining elements
    for (; i < k; ++i) {
      sum += static_cast<uint64_t>(tmp[i]);
    }
  }
  
  return sum;
}