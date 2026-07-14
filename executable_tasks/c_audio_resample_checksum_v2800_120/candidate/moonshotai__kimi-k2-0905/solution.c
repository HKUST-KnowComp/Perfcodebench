#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    uint64_t hash = 0;
    const int64_t scale_fp = (int64_t)(input_len - 1) * 2147483648LL; // 2^31
    const int64_t inv_output_len_minus_1 = (int64_t)((1.0 / (double)(output_len - 1)) * 2147483648LL);
    int16_t* out = (int16_t*)_mm_malloc((size_t)output_len * sizeof(int16_t), 32);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        int i = 0;
        for (; i <= output_len - 8; i += 8) {
            __m128i idx0, idx1;
            __m128i frac0, frac1;
            __m128i sample_lo, sample_hi;
            __m128i lo, hi;

            // compute positions for 8 outputs
            for (int k = 0; k < 8; ++k) {
                int64_t pos_fp = (int64_t)(i + k) * scale_fp;
                int pos_int = (int)(pos_fp >> 31);
                int frac_val = (int)(pos_fp & 0x7FFFFFFF);
                idx0 = _mm_insert_epi16(idx0, (pos_int < input_len - 1) ? pos_int : input_len - 1, k);
                idx1 = _mm_insert_epi16(idx1, (pos_int + 1 < input_len - 1) ? pos_int + 1 : input_len - 1, k);
                frac0 = _mm_insert_epi16(frac0, frac_val >> 15, k);
                frac1 = _mm_insert_epi16(frac1, 0x8000 - frac_val >> 15, k);
            }

            // gather src samples
            for (int k = 0; k < 8; ++k) {
                lo = _mm_set1_epi16(src[_mm_extract_epi16(idx0, k)]);
                hi = _mm_set1_epi16(src[_mm_extract_epi16(idx1, k)]);
                lo = _mm_mullo_epi16(lo, _mm_insert_epi16(sample_lo, _mm_extract_epi16(frac1, k), k));
                hi = _mm_mullo_epi16(hi, _mm_insert_epi16(sample_hi, _mm_extract_epi16(frac0, k), k));
                lo = _mm_add_epi16(lo, hi);
                lo = _mm_srli_epi16(lo, 15);
                idx0 = _mm_insert_epi16(idx0, _mm_extract_epi16(lo, 0), k);
            }
            _mm_store_si128((__m128i*)(out + i), idx0);
        }
        // scalar tail
        for (; i < output_len; ++i) {
            int64_t pos_fp = (int64_t)i * scale_fp;
            int pos_int = (int)(pos_fp >> 31);
            int frac_val = (int)(pos_fp & 0x7FFFFFFF);
            int left  = (pos_int < input_len - 1) ? pos_int : input_len - 1;
            int right = (pos_int + 1 < input_len - 1) ? pos_int + 1 : input_len - 1;
            int a = src[left];
            int b = src[right];
            int c = (a * (0x8000 - (frac_val >> 15)) + b * (frac_val >> 15)) >> 15;
            out[i] = (int16_t)c;
        }
        hash = checksum_i16(out, (size_t)output_len);
    }
    _mm_free(out);
    return hash;
}