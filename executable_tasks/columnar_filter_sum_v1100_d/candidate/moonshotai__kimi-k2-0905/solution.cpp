#include "interface.h"
#include <cstdint>
#include <vector>
#include <emmintrin.h>   // SSE2

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters)
{
    const std::size_t n = keys.size();
    const uint32_t*  k = keys.data();
    const uint32_t*  v = values.data();
    const uint8_t*   f = flags.data();

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        __m128i vk, vv, vf, mask, cmp, bits, pass;
        __m128i sum128 = _mm_setzero_si128();
        __m128i thresh = _mm_set1_epi32(static_cast<int32_t>(key_threshold));

        std::size_t i = 0;
        // 4-way SIMD main loop
        for (; i + 4 <= n; i += 4) {
            vk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(k + i));
            vv = _mm_loadu_si128(reinterpret_cast<const __m128i*>(v + i));

            // flags: load 4 bytes, zero-extend to 32-bit lanes
            __m128i f8  = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f + i));
            vf = _mm_unpacklo_epi8(f8, _mm_setzero_si128());
            vf = _mm_unpacklo_epi16(vf, _mm_setzero_si128());

            // predicate: flag && key < threshold && (key & 3) == 0
            cmp   = _mm_cmplt_epi32(vk, thresh);                 // key < threshold
            bits  = _mm_and_si128(vk, _mm_set1_epi32(3));        // key & 3
            pass  = _mm_cmpeq_epi32(bits, _mm_setzero_si128());  // (key & 3) == 0
            pass  = _mm_and_si128(pass, cmp);                    // key < threshold && (key & 3) == 0
            pass  = _mm_and_si128(pass, vf);                     // flag && …

            // blend: 0 or value
            __m128i addv = _mm_and_si128(vv, pass);
            sum128 = _mm_add_epi64(sum128,
                         _mm_add_epi64(_mm_unpacklo_epi32(addv, _mm_setzero_si128()),
                                       _mm_unpacklo_epi32(_mm_srli_si128(addv, 8), _mm_setzero_si128())));
        }

        // horizontal sum of 2×64-bit lanes
        uint64_t partial = static_cast<uint64_t>(_mm_cvtsi128_si64(sum128)) +
                           static_cast<uint64_t>(_mm_cvtsi128_si64(_mm_srli_si128(sum128, 8)));

        // scalar tail
        for (; i < n; ++i) {
            if (f[i] && k[i] < key_threshold && ((k[i] & 3u) == 0u))
                partial += v[i];
        }

        total = partial;   // final sum of last iteration
    }
    return total;
}