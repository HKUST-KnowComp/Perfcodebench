#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>
#include <algorithm>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters)
{
    const std::size_t M = taps.size();
    const std::size_t N = signal.size();
    if (M == 0 || N == 0 || iters == 0) return 0;

    // Reverse taps once and zero-pad so we can always do 16-element loads
    alignas(32) std::vector<int16_t> rev_taps_padded(M + 31, 0);
    for (std::size_t i = 0; i < M; ++i)
        rev_taps_padded[i] = taps[M - 1 - i];

    // Allocate reusable output buffer
    std::vector<int64_t> output(N);

    const __m256i *ptaps = reinterpret_cast<const __m256i*>(rev_taps_padded.data());

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(output.begin(), output.end(), 0);

        // FIR: outer loop walks over taps, inner loop over signal (cache-friendly)
        for (std::size_t k = 0; k < M; ++k) {
            const int16_t tap = rev_taps_padded[k];
            const __m256i vt = _mm256_set1_epi16(tap);
            std::size_t i = 0;

            // Process 16 samples at a time
            for (; i + 15 < N - k; i += 16) {
                __m256i vs = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&signal[i]));
                __m256i prod = _mm256_madd_epi16(vs, vt); // 16×16→32 multiply-add
                __m256i lo   = _mm256_unpacklo_epi32(prod, _mm256_setzero_si256());
                __m256i hi   = _mm256_unpackhi_epi32(prod, _mm256_setzero_si256());
                __m256i vout_lo = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&output[i + k]));
                __m256i vout_hi = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&output[i + k + 8]));
                vout_lo = _mm256_add_epi64(vout_lo, lo);
                vout_hi = _mm256_add_epi64(vout_hi, hi);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(&output[i + k]), vout_lo);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(&output[i + k + 8]), vout_hi);
            }
            // Scalar tail
            for (; i < N - k; ++i)
                output[i + k] += static_cast<int64_t>(signal[i]) * static_cast<int64_t>(tap);
        }

        // Compute checksum
        uint64_t total = 1469598103934665603ULL;
        for (std::size_t i = 0; i < N; ++i) {
            int64_t acc = output[i];
            uint64_t v = static_cast<uint64_t>(acc * acc + 3 * acc);
            total ^= v;
            total *= 1099511628211ULL;
        }
        if (iter == iters - 1) return total;
    }
    return 0;
}