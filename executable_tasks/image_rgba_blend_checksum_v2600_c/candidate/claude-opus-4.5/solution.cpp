#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE2 1
#if defined(__AVX2__)
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif
#else
#define HAS_SSE2 0
#define HAS_AVX2 0
#endif

namespace {

alignas(64) uint16_t div255_table[256 * 256];
bool table_initialized = false;

void init_div255_table() {
    if (table_initialized) return;
    for (int i = 0; i < 256 * 256; ++i) {
        div255_table[i] = static_cast<uint16_t>((i + 127) / 255);
    }
    table_initialized = true;
}

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a);
    return static_cast<uint8_t>(div255_table[val]);
}

inline uint8_t div255_approx(uint32_t x) {
    return static_cast<uint8_t>((x + 127 + ((x + 127) >> 8)) >> 8);
}

inline uint8_t blend_byte_approx(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a);
    return div255_approx(val);
}

#if HAS_SSE2
inline __m128i div255_sse(__m128i x) {
    __m128i c127 = _mm_set1_epi16(127);
    __m128i t = _mm_add_epi16(x, c127);
    __m128i t2 = _mm_srli_epi16(t, 8);
    t = _mm_add_epi16(t, t2);
    return _mm_srli_epi16(t, 8);
}

void blend_pixels_sse2(const uint8_t* __restrict src, const uint8_t* __restrict dst, 
                       uint8_t* __restrict out, std::size_t pixels) {
    std::size_t i = 0;
    
    for (; i + 4 <= pixels; i += 4) {
        __m128i src_pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i * 4));
        __m128i dst_pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i * 4));
        
        __m128i zero = _mm_setzero_si128();
        __m128i c255 = _mm_set1_epi16(255);
        __m128i alpha_mask = _mm_set1_epi32(0xFF000000);
        
        __m128i src_lo = _mm_unpacklo_epi8(src_pixels, zero);
        __m128i src_hi = _mm_unpackhi_epi8(src_pixels, zero);
        __m128i dst_lo = _mm_unpacklo_epi8(dst_pixels, zero);
        __m128i dst_hi = _mm_unpackhi_epi8(dst_pixels, zero);
        
        __m128i alpha_lo = _mm_shufflelo_epi16(src_lo, _MM_SHUFFLE(3, 3, 3, 3));
        alpha_lo = _mm_shufflehi_epi16(alpha_lo, _MM_SHUFFLE(3, 3, 3, 3));
        __m128i alpha_hi = _mm_shufflelo_epi16(src_hi, _MM_SHUFFLE(3, 3, 3, 3));
        alpha_hi = _mm_shufflehi_epi16(alpha_hi, _MM_SHUFFLE(3, 3, 3, 3));
        
        __m128i inv_alpha_lo = _mm_sub_epi16(c255, alpha_lo);
        __m128i inv_alpha_hi = _mm_sub_epi16(c255, alpha_hi);
        
        __m128i blend_lo = _mm_add_epi16(_mm_mullo_epi16(src_lo, alpha_lo), 
                                          _mm_mullo_epi16(dst_lo, inv_alpha_lo));
        __m128i blend_hi = _mm_add_epi16(_mm_mullo_epi16(src_hi, alpha_hi), 
                                          _mm_mullo_epi16(dst_hi, inv_alpha_hi));
        
        blend_lo = div255_sse(blend_lo);
        blend_hi = div255_sse(blend_hi);
        
        __m128i result = _mm_packus_epi16(blend_lo, blend_hi);
        result = _mm_or_si128(_mm_andnot_si128(alpha_mask, result), alpha_mask);
        
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i * 4), result);
    }
    
    for (; i < pixels; ++i) {
        std::size_t idx = i * 4;
        uint8_t a = src[idx + 3];
        out[idx] = blend_byte_approx(src[idx], dst[idx], a);
        out[idx + 1] = blend_byte_approx(src[idx + 1], dst[idx + 1], a);
        out[idx + 2] = blend_byte_approx(src[idx + 2], dst[idx + 2], a);
        out[idx + 3] = 255u;
    }
}
#endif

void blend_pixels_scalar(const uint8_t* __restrict src, const uint8_t* __restrict dst, 
                         uint8_t* __restrict out, std::size_t pixels) {
    for (std::size_t i = 0; i < pixels; ++i) {
        std::size_t idx = i * 4;
        uint8_t a = src[idx + 3];
        out[idx] = blend_byte_approx(src[idx], dst[idx], a);
        out[idx + 1] = blend_byte_approx(src[idx + 1], dst[idx + 1], a);
        out[idx + 2] = blend_byte_approx(src[idx + 2], dst[idx + 2], a);
        out[idx + 3] = 255u;
    }
}

uint64_t checksum_bytes_fast(const uint8_t* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;
    
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
    
    const std::size_t pixels = src.size() / 4u;
    const std::size_t out_size = pixels * 4u;
    
    std::vector<uint8_t> out(out_size);
    
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();
    uint8_t* out_ptr = out.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
#if HAS_SSE2
        blend_pixels_sse2(src_ptr, dst_ptr, out_ptr, pixels);
#else
        blend_pixels_scalar(src_ptr, dst_ptr, out_ptr, pixels);
#endif
        hash = checksum_bytes_fast(out_ptr, out_size);
    }
    
    return hash;
}