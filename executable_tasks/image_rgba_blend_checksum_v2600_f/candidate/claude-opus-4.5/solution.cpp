#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

#if defined(__SSE4_1__) || defined(__x86_64__) || defined(_M_X64)
#include <emmintrin.h>
#include <smmintrin.h>
#define USE_SSE 1
#else
#define USE_SSE 0
#endif

namespace {

inline uint8_t blend_byte(uint32_t s, uint32_t d, uint32_t a, uint32_t inv_a) {
    return static_cast<uint8_t>((s * a + d * inv_a + 127u) / 255u);
}

inline uint64_t checksum_bytes_fast(const uint8_t* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    std::size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 3]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 4]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 5]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 6]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 7]);
        hash *= prime;
    }
    for (; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    (void)width;
    (void)height;
    
    const std::size_t total_bytes = src.size();
    const std::size_t pixels = total_bytes / 4u;
    const std::size_t out_size = pixels * 4u;
    
    std::vector<uint8_t> out(out_size);
    uint8_t* out_ptr = out.data();
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::size_t i = 0;
        
#if USE_SSE
        const __m128i v255 = _mm_set1_epi16(255);
        const __m128i v127 = _mm_set1_epi16(127);
        const __m128i vFF = _mm_set1_epi8((char)0xFF);
        
        for (; i + 4 <= pixels; i += 4) {
            std::size_t idx = i * 4;
            
            __m128i src_rgba = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_ptr + idx));
            __m128i dst_rgba = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst_ptr + idx));
            
            uint8_t a0 = src_ptr[idx + 3];
            uint8_t a1 = src_ptr[idx + 7];
            uint8_t a2 = src_ptr[idx + 11];
            uint8_t a3 = src_ptr[idx + 15];
            
            __m128i alpha = _mm_setr_epi8(
                a0, a0, a0, 0,
                a1, a1, a1, 0,
                a2, a2, a2, 0,
                a3, a3, a3, 0
            );
            
            __m128i inv_alpha = _mm_sub_epi8(vFF, alpha);
            inv_alpha = _mm_or_si128(inv_alpha, _mm_setr_epi8(0,0,0,(char)0xFF,0,0,0,(char)0xFF,0,0,0,(char)0xFF,0,0,0,(char)0xFF));
            alpha = _mm_or_si128(alpha, _mm_setr_epi8(0,0,0,(char)0xFF,0,0,0,(char)0xFF,0,0,0,(char)0xFF,0,0,0,(char)0xFF));
            
            __m128i src_lo = _mm_unpacklo_epi8(src_rgba, _mm_setzero_si128());
            __m128i src_hi = _mm_unpackhi_epi8(src_rgba, _mm_setzero_si128());
            __m128i dst_lo = _mm_unpacklo_epi8(dst_rgba, _mm_setzero_si128());
            __m128i dst_hi = _mm_unpackhi_epi8(dst_rgba, _mm_setzero_si128());
            __m128i alpha_lo = _mm_unpacklo_epi8(alpha, _mm_setzero_si128());
            __m128i alpha_hi = _mm_unpackhi_epi8(alpha, _mm_setzero_si128());
            __m128i inv_alpha_lo = _mm_unpacklo_epi8(inv_alpha, _mm_setzero_si128());
            __m128i inv_alpha_hi = _mm_unpackhi_epi8(inv_alpha, _mm_setzero_si128());
            
            __m128i prod_lo = _mm_add_epi16(_mm_mullo_epi16(src_lo, alpha_lo), _mm_mullo_epi16(dst_lo, inv_alpha_lo));
            __m128i prod_hi = _mm_add_epi16(_mm_mullo_epi16(src_hi, alpha_hi), _mm_mullo_epi16(dst_hi, inv_alpha_hi));
            
            prod_lo = _mm_add_epi16(prod_lo, v127);
            prod_hi = _mm_add_epi16(prod_hi, v127);
            
            __m128i tmp_lo = _mm_add_epi16(prod_lo, _mm_srli_epi16(prod_lo, 8));
            __m128i tmp_hi = _mm_add_epi16(prod_hi, _mm_srli_epi16(prod_hi, 8));
            __m128i res_lo = _mm_srli_epi16(tmp_lo, 8);
            __m128i res_hi = _mm_srli_epi16(tmp_hi, 8);
            
            __m128i result = _mm_packus_epi16(res_lo, res_hi);
            
            result = _mm_or_si128(_mm_and_si128(result, _mm_setr_epi8((char)0xFF,(char)0xFF,(char)0xFF,0,(char)0xFF,(char)0xFF,(char)0xFF,0,(char)0xFF,(char)0xFF,(char)0xFF,0,(char)0xFF,(char)0xFF,(char)0xFF,0)),
                                  _mm_setr_epi8(0,0,0,(char)0xFF,0,0,0,(char)0xFF,0,0,0,(char)0xFF,0,0,0,(char)0xFF));
            
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out_ptr + idx), result);
        }
#endif
        
        for (; i < pixels; ++i) {
            const std::size_t idx = i * 4u;
            const uint32_t a = src_ptr[idx + 3u];
            const uint32_t inv_a = 255u - a;
            out_ptr[idx] = blend_byte(src_ptr[idx], dst_ptr[idx], a, inv_a);
            out_ptr[idx + 1] = blend_byte(src_ptr[idx + 1], dst_ptr[idx + 1], a, inv_a);
            out_ptr[idx + 2] = blend_byte(src_ptr[idx + 2], dst_ptr[idx + 2], a, inv_a);
            out_ptr[idx + 3] = 255u;
        }
        
        hash = checksum_bytes_fast(out_ptr, out_size);
    }
    
    return hash;
}