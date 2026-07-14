#include <stddef.h>
#include <stdint.h>
#include <immintrin.h>

static inline uint64_t scalar_tail(const uint32_t *ids,
                                   const uint32_t *dict_vals,
                                   size_t off, size_t n, uint64_t h)
{
    for (size_t i = off; i < n; ++i) {
        uint64_t v = dict_vals[ids[i]] + 1ULL;
        h ^= v;
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const uint32_t *ids, size_t n,
             const uint32_t *dict_vals, uint32_t dict_size,
             int iters)
{
    (void)dict_size;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        size_t i = 0;

#if defined(__AVX512F__)
        const __m512i vmul = _mm512_set1_epi64(1099511628211ULL);
        const __m512i vinc = _mm512_set1_epi32(1);
        __m512i acc0 = _mm512_setzero_si512();
        __m512i acc1 = _mm512_setzero_si512();
        __m512i acc2 = _mm512_setzero_si512();
        __m512i acc3 = _mm512_setzero_si512();

        for (; i + 16 <= n; i += 16) {
            __m512i idx = _mm512_loadu_si512((__m512i const*)(ids + i));
            __m512i v0 = _mm512_i32gather_epi32(idx, dict_vals, 4);
            v0 = _mm512_add_epi32(v0, vinc);
            __m512i v1 = _mm512_xor_si512(v0, _mm512_castpd_si512(_mm512_castsi512_pd(acc0)));
            __m512i v2 = _mm512_mullo_epi64(v1, vmul);
            acc0 = v2;
        }
        uint64_t h_vec[8];
        _mm512_storeu_si512((__m512i*)h_vec, acc0);
        for (unsigned k = 0; k < 8; ++k) {
            h ^= h_vec[k];
            h *= 1099511628211ULL;
        }
#elif defined(__AVX2__)
        const __m256i vmul = _mm256_set1_epi64x(1099511628211ULL);
        const __m256i vinc = _mm256_set1_epi32(1);
        __m256i acc0 = _mm256_setzero_si256();
        __m256i acc1 = _mm256_setzero_si256();

        for (; i + 8 <= n; i += 8) {
            __m256i idx = _mm256_loadu_si256((__m256i const*)(ids + i));
            __m256i v0 = _mm256_i32gather_epi32((const int*)dict_vals, idx, 4);
            v0 = _mm256_add_epi32(v0, vinc);
            __m256i v1 = _mm256_xor_si256(v0, acc0);
            __m256i v2 = _mm256_mullo_epi64(v1, vmul);
            acc0 = v2;
        }
        uint64_t h_vec[4];
        _mm256_storeu_si256((__m256i*)h_vec, acc0);
        for (unsigned k = 0; k < 4; ++k) {
            h ^= h_vec[k];
            h *= 1099511628211ULL;
        }
#else
        const uint64_t kMul = 1099511628211ULL;
        for (; i + 4 <= n; i += 4) {
            uint64_t v0 = dict_vals[ids[i+0]] + 1ULL;
            uint64_t v1 = dict_vals[ids[i+1]] + 1ULL;
            uint64_t v2 = dict_vals[ids[i+2]] + 1ULL;
            uint64_t v3 = dict_vals[ids[i+3]] + 1ULL;

            h ^= v0; h *= kMul;
            h ^= v1; h *= kMul;
            h ^= v2; h *= kMul;
            h ^= v3; h *= kMul;
        }
#endif
        h = scalar_tail(ids, dict_vals, i, n, h);
        if (iter == iters - 1) return h;
    }
    return 0;
}