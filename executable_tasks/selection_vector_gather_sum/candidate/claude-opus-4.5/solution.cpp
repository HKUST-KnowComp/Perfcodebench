#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {

    const std::size_t n = keys.size();
    const uint32_t* __restrict keys_ptr = keys.data();
    const uint8_t* __restrict flags_ptr = flags.data();
    const uint32_t* __restrict payloads_ptr = payloads.data();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;

#if defined(__AVX2__)
        const __m256i thresh_vec = _mm256_set1_epi32(static_cast<int>(threshold));
        const __m256i mask7 = _mm256_set1_epi32(7);
        const __m256i zero = _mm256_setzero_si256();

        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();

        std::size_t i = 0;
        const std::size_t vec_end = (n >= 8) ? (n - 7) : 0;

        for (; i < vec_end; i += 8) {
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys_ptr + i));

            // Load 8 flags (as bytes), zero-extend to 32-bit
            __m128i f8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(flags_ptr + i));
            __m256i f32 = _mm256_cvtepu8_epi32(f8);

            // flags != 0
            __m256i cmp_flags = _mm256_cmpgt_epi32(f32, zero);

            // keys < threshold (unsigned compare via signed trick: both < 2^31)
            // For unsigned compare: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) signed
            // But if threshold and keys are < 2^31, signed compare works directly.
            // We'll use a different approach: a < b unsigned iff max(a,b) != a
            // Or: ~(a >= b) = ~(a == max(a,b))
            // Simpler: use _mm256_cmpgt for signed, but we need unsigned.
            // AVX2 doesn't have unsigned cmplt. Use: a < b <=> (a - b) has high bit set (borrow)
            // Or: a < b <=> ~(a >= b) <=> ~(max_u(a,b) == a)
            __m256i max_kt = _mm256_max_epu32(k, thresh_vec);
            __m256i cmp_thresh = _mm256_cmpeq_epi32(max_kt, thresh_vec); // k <= threshold-1 means max(k, thresh) != k, but we want k < thresh
            // Actually: k < thresh <=> max(k, thresh) == thresh AND k != thresh
            // Simpler: k < thresh <=> min(k, thresh) == k AND k != thresh
            // Even simpler for < : k < thresh <=> max(k, thresh) != k
            // cmp_thresh currently is: max(k, thresh) == thresh, which means k <= thresh
            // We want k < thresh, so also need k != thresh
            __m256i eq_thresh = _mm256_cmpeq_epi32(k, thresh_vec);
            cmp_thresh = _mm256_andnot_si256(eq_thresh, cmp_thresh); // k < thresh

            // (keys & 7) == 0
            __m256i k_and_7 = _mm256_and_si256(k, mask7);
            __m256i cmp_mod = _mm256_cmpeq_epi32(k_and_7, zero);

            // Combined mask
            __m256i mask = _mm256_and_si256(cmp_flags, _mm256_and_si256(cmp_thresh, cmp_mod));

            // Gather payloads (indices are i, i+1, ..., i+7)
            __m256i indices = _mm256_setr_epi32(
                static_cast<int>(i), static_cast<int>(i+1), static_cast<int>(i+2), static_cast<int>(i+3),
                static_cast<int>(i+4), static_cast<int>(i+5), static_cast<int>(i+6), static_cast<int>(i+7));

            // We don't actually need gather since indices are contiguous - just load
            __m256i p = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(payloads_ptr + i));

            // Apply mask (zero out non-matching)
            p = _mm256_and_si256(p, mask);

            // Accumulate as 64-bit to avoid overflow
            // Split into low and high 4 elements, zero-extend to 64-bit
            __m128i p_lo = _mm256_castsi256_si128(p);
            __m128i p_hi = _mm256_extracti128_si256(p, 1);

            __m256i p_lo_64 = _mm256_cvtepu32_epi64(p_lo);
            __m256i p_hi_64 = _mm256_cvtepu32_epi64(p_hi);

            sum_lo = _mm256_add_epi64(sum_lo, p_lo_64);
            sum_hi = _mm256_add_epi64(sum_hi, p_hi_64);
        }

        // Horizontal sum of sum_lo and sum_hi
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        __m128i total_lo = _mm256_castsi256_si128(total);
        __m128i total_hi = _mm256_extracti128_si256(total, 1);
        __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
        sum128 = _mm_add_epi64(sum128, _mm_srli_si128(sum128, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(sum128));

        // Scalar tail
        for (; i < n; ++i) {
            uint32_t k = keys_ptr[i];
            if (flags_ptr[i] && k < threshold && ((k & 7u) == 0u)) {
                sum += static_cast<uint64_t>(payloads_ptr[i]);
            }
        }
#else
        // Scalar fallback
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t k = keys_ptr[i];
            if (flags_ptr[i] && k < threshold && ((k & 7u) == 0u)) {
                sum += static_cast<uint64_t>(payloads_ptr[i]);
            }
        }
#endif
    }

    return sum;
}
