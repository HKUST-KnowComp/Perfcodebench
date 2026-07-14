#include "interface.h"
#include <cstdint>
#include <immintrin.h>
#include <vector>

/**
 * Optimized Sobel edge extraction using AVX2 and integrated FNV-1a checksum.
 */
uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  
  if (height < 3 || width < 3) return 1469598103934665603ULL;

  uint64_t final_hash = 0;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET_BASIS;

    for (int y = 1; y < height - 1; ++y) {
      const uint8_t* row_prev = &image[(y - 1) * width];
      const uint8_t* row_curr = &image[y * width];
      const uint8_t* row_next = &image[(y + 1) * width];

      int x = 1;
      // AVX2 processing: 32 pixels at a time
      for (; x <= width - 33; x += 32) {
        __m256i p0 = _mm256_loadu_si256((const __m256i*)(row_prev + x - 1));
        __m256i p1 = _mm256_loadu_si256((const __m256i*)(row_prev + x));
        __m256i p2 = _mm256_loadu_si256((const __m256i*)(row_prev + x + 1));
        __m256i p3 = _mm256_loadu_si256((const __m256i*)(row_curr + x - 1));
        __m256i p5 = _mm256_loadu_si256((const __m256i*)(row_curr + x + 1));
        __m256i p6 = _mm256_loadu_si256((const __m256i*)(row_next + x - 1));
        __m256i p7 = _mm256_loadu_si256((const __m256i*)(row_next + x));
        __m256i p8 = _mm256_loadu_si256((const __m256i*)(row_next + x + 1));

        auto calc_mag = [&](__m128i v0, __m128i v1, __m128i v2, __m128i v3, __m128i v5, __m128i v6, __m128i v7, __m128i v8) {
          __m128i zero = _mm_setzero_si128();
          auto to_i16_l = [&](__m128i v) { return _mm_unpacklo_epi8(v, zero); };
          auto to_i16_h = [&](__m128i v) { return _mm_unpackhi_epi8(v, zero); };

          auto process_half = [&](__m128i h0, __m128i h1, __m128i h2, __m128i h3, __m128i h5, __m128i h6, __m128i h7, __m128i h8) {
            // gx = -p0 + p2 - 2*p3 + 2*p5 - p6 + p8
            __m128i gx = _mm_sub_epi16(h2, h0);
            gx = _mm_add_epi16(gx, _mm_slli_epi16(_mm_sub_epi16(h5, h3), 1));
            gx = _mm_add_epi16(gx, _mm_sub_epi16(h8, h6));
            
            // gy = p0 + 2*p1 + p2 - p6 - 2*p7 - p8
            __m128i gy = _mm_add_epi16(h0, h2);
            gy = _mm_add_epi16(gy, _mm_slli_epi16(_mm_sub_epi16(h1, h7), 1));
            gy = _mm_sub_epi16(gy, _mm_add_epi16(h6, h8));

            __m128i abs_gx = _mm_abs_epi16(gx);
            __m128i abs_gy = _mm_abs_epi16(gy);
            return _mm_packus_epi16(_mm_adds_epu16(abs_gx, abs_gy), zero);
          };

          __m128i res_l = process_half(to_i16_l(v0), to_i16_l(v1), to_i16_l(v2), to_i16_l(v3), to_i16_l(v5), to_i16_l(v6), to_i16_l(v7), to_i16_l(v8));
          __m128i res_h = process_half(to_i16_h(v0), to_i16_h(v1), to_i16_h(v2), to_i16_h(v3), to_i16_h(v5), to_i16_h(v6), to_i16_h(v7), to_i16_h(v8));
          return _mm_unpacklo_epi64(res_l, res_h);
        };

        __m128i mag_l = calc_mag(_mm256_extracti128_si256(p0, 0), _mm256_extracti128_si256(p1, 0), _mm256_extracti128_si256(p2, 0), _mm256_extracti128_si256(p3, 0), _mm256_extracti128_si256(p5, 0), _mm256_extracti128_si256(p6, 0), _mm256_extracti128_si256(p7, 0), _mm256_extracti128_si256(p8, 0));
        __m128i mag_h = calc_mag(_mm256_extracti128_si256(p0, 1), _mm256_extracti128_si256(p1, 1), _mm256_extracti128_si256(p2, 1), _mm256_extracti128_si256(p3, 1), _mm256_extracti128_si256(p5, 1), _mm256_extracti128_si256(p6, 1), _mm256_extracti128_si256(p7, 1), _mm256_extracti128_si256(p8, 1));

        alignas(32) uint8_t buffer[32];
        _mm_storeu_si128((__m128i*)buffer, mag_l);
        _mm_storeu_si128((__m128i*)(buffer + 16), mag_h);

        for (int i = 0; i < 32; ++i) {
          hash ^= static_cast<uint64_t>(buffer[i]);
          hash *= FNV_PRIME;
        }
      }

      // Scalar fallback for remaining pixels in row
      for (; x < width - 1; ++x) {
        const int gx = -row_prev[x - 1] + row_prev[x + 1] - 2 * row_curr[x - 1] + 2 * row_curr[x + 1] - row_next[x - 1] + row_next[x + 1];
        const int gy = row_prev[x - 1] + 2 * row_prev[x] + row_prev[x + 1] - row_next[x - 1] - 2 * row_next[x] - row_next[x + 1];
        int mag = (gx < 0 ? -gx : gx) + (gy < 0 ? -gy : gy);
        uint8_t res = static_cast<uint8_t>(mag > 255 ? 255 : mag);
        hash ^= static_cast<uint64_t>(res);
        hash *= FNV_PRIME;
      }
    }
    final_hash = hash;
  }

  return final_hash;
}
