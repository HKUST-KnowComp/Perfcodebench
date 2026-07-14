#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t prime = 1099511628211ULL;
  const uint64_t offset_basis = 1469598103934665603ULL;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash0 = offset_basis;
    uint64_t hash1 = offset_basis;
    uint64_t hash2 = offset_basis;
    uint64_t hash3 = offset_basis;
    uint64_t hash4 = offset_basis;
    uint64_t hash5 = offset_basis;
    uint64_t hash6 = offset_basis;
    uint64_t hash7 = offset_basis;

    size_t i = 0;
    const size_t vec_step = 4;
    const size_t unroll = 8;
    const size_t simd_bound = (n / (vec_step * unroll)) * (vec_step * unroll);

    for (; i < simd_bound; i += vec_step * unroll) {
      __m256i id0 = _mm256_loadu_si256((const __m256i*)(ids + i + 0 * vec_step));
      __m256i id1 = _mm256_loadu_si256((const __m256i*)(ids + i + 1 * vec_step));
      __m256i id2 = _mm256_loadu_si256((const __m256i*)(ids + i + 2 * vec_step));
      __m256i id3 = _mm256_loadu_si256((const __m256i*)(ids + i + 3 * vec_step));
      __m256i id4 = _mm256_loadu_si256((const __m256i*)(ids + i + 4 * vec_step));
      __m256i id5 = _mm256_loadu_si256((const __m256i*)(ids + i + 5 * vec_step));
      __m256i id6 = _mm256_loadu_si256((const __m256i*)(ids + i + 6 * vec_step));
      __m256i id7 = _mm256_loadu_si256((const __m256i*)(ids + i + 7 * vec_step));

      __m256i v0 = _mm256_i32gather_epi32((const int*)dict_vals, id0, 4);
      __m256i v1 = _mm256_i32gather_epi32((const int*)dict_vals, id1, 4);
      __m256i v2 = _mm256_i32gather_epi32((const int*)dict_vals, id2, 4);
      __m256i v3 = _mm256_i32gather_epi32((const int*)dict_vals, id3, 4);
      __m256i v4 = _mm256_i32gather_epi32((const int*)dict_vals, id4, 4);
      __m256i v5 = _mm256_i32gather_epi32((const int*)dict_vals, id5, 4);
      __m256i v6 = _mm256_i32gather_epi32((const int*)dict_vals, id6, 4);
      __m256i v7 = _mm256_i32gather_epi32((const int*)dict_vals, id7, 4);

      hash0 ^= (uint64_t)_mm256_extract_epi32(v0, 0) + 1; hash0 *= prime;
      hash0 ^= (uint64_t)_mm256_extract_epi32(v0, 1) + 1; hash0 *= prime;
      hash0 ^= (uint64_t)_mm256_extract_epi32(v0, 2) + 1; hash0 *= prime;
      hash0 ^= (uint64_t)_mm256_extract_epi32(v0, 3) + 1; hash0 *= prime;

      hash1 ^= (uint64_t)_mm256_extract_epi32(v1, 0) + 1; hash1 *= prime;
      hash1 ^= (uint64_t)_mm256_extract_epi32(v1, 1) + 1; hash1 *= prime;
      hash1 ^= (uint64_t)_mm256_extract_epi32(v1, 2) + 1; hash1 *= prime;
      hash1 ^= (uint64_t)_mm256_extract_epi32(v1, 3) + 1; hash1 *= prime;

      hash2 ^= (uint64_t)_mm256_extract_epi32(v2, 0) + 1; hash2 *= prime;
      hash2 ^= (uint64_t)_mm256_extract_epi32(v2, 1) + 1; hash2 *= prime;
      hash2 ^= (uint64_t)_mm256_extract_epi32(v2, 2) + 1; hash2 *= prime;
      hash2 ^= (uint64_t)_mm256_extract_epi32(v2, 3) + 1; hash2 *= prime;

      hash3 ^= (uint64_t)_mm256_extract_epi32(v3, 0) + 1; hash3 *= prime;
      hash3 ^= (uint64_t)_mm256_extract_epi32(v3, 1) + 1; hash3 *= prime;
      hash3 ^= (uint64_t)_mm256_extract_epi32(v3, 2) + 1; hash3 *= prime;
      hash3 ^= (uint64_t)_mm256_extract_epi32(v3, 3) + 1; hash3 *= prime;

      hash4 ^= (uint64_t)_mm256_extract_epi32(v4, 0) + 1; hash4 *= prime;
      hash4 ^= (uint64_t)_mm256_extract_epi32(v4, 1) + 1; hash4 *= prime;
      hash4 ^= (uint64_t)_mm256_extract_epi32(v4, 2) + 1; hash4 *= prime;
      hash4 ^= (uint64_t)_mm256_extract_epi32(v4, 3) + 1; hash4 *= prime;

      hash5 ^= (uint64_t)_mm256_extract_epi32(v5, 0) + 1; hash5 *= prime;
      hash5 ^= (uint64_t)_mm256_extract_epi32(v5, 1) + 1; hash5 *= prime;
      hash5 ^= (uint64_t)_mm256_extract_epi32(v5, 2) + 1; hash5 *= prime;
      hash5 ^= (uint64_t)_mm256_extract_epi32(v5, 3) + 1; hash5 *= prime;

      hash6 ^= (uint64_t)_mm256_extract_epi32(v6, 0) + 1; hash6 *= prime;
      hash6 ^= (uint64_t)_mm256_extract_epi32(v6, 1) + 1; hash6 *= prime;
      hash6 ^= (uint64_t)_mm256_extract_epi32(v6, 2) + 1; hash6 *= prime;
      hash6 ^= (uint64_t)_mm256_extract_epi32(v6, 3) + 1; hash6 *= prime;

      hash7 ^= (uint64_t)_mm256_extract_epi32(v7, 0) + 1; hash7 *= prime;
      hash7 ^= (uint64_t)_mm256_extract_epi32(v7, 1) + 1; hash7 *= prime;
      hash7 ^= (uint64_t)_mm256_extract_epi32(v7, 2) + 1; hash7 *= prime;
      hash7 ^= (uint64_t)_mm256_extract_epi32(v7, 3) + 1; hash7 *= prime;
    }

    for (; i < n; ++i) {
      uint32_t val = dict_vals[ids[i]];
      hash0 ^= (uint64_t)val + 1;
      hash0 *= prime;
    }

    h = hash0 ^ hash1 ^ hash2 ^ hash3 ^ hash4 ^ hash5 ^ hash6 ^ hash7;
  }
  return h;
}