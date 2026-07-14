#include "interface.h"

#include <cstdint>
#include <vector>
#include <emmintrin.h>

namespace {

inline uint64_t checksum_bytes_fast(const uint8_t* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    std::size_t i = 0;
    std::size_t len8 = len & ~7ULL;
    
    for (; i < len8; i += 8) {
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
    
    std::vector<uint8_t> out(total_bytes);
    uint64_t hash = 0;
    
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();
    uint8_t* out_ptr = out.data();
    
    const __m128i v127 = _mm_set1_epi16(127);
    const __m128i v255_16 = _mm_set1_epi16(255);
    const __m128i zero = _mm_setzero_si128();
    
    for (int iter = 0; iter < iters; ++iter) {
        std::size_t i = 0;
        std::size_t pixels4 = (pixels / 4) * 4;
        
        for (; i < pixels4; i += 4) {
            std::size_t idx = i * 4;
            
            // Load 4 pixels (16 bytes) from src and dst
            __m128i src_bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_ptr + idx));
            __m128i dst_bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst_ptr + idx));
            
            // Extract alpha values for each pixel
            uint8_t a0 = src_ptr[idx + 3];
            uint8_t a1 = src_ptr[idx + 7];
            uint8_t a2 = src_ptr[idx + 11];
            uint8_t a3 = src_ptr[idx + 15];
            
            // Create alpha vector: [a0,a0,a0,255, a1,a1,a1,255, a2,a2,a2,255, a3,a3,a3,255]
            __m128i alpha = _mm_set_epi8(
                (char)255, (char)a3, (char)a3, (char)a3,
                (char)255, (char)a2, (char)a2, (char)a2,
                (char)255, (char)a1, (char)a1, (char)a1,
                (char)255, (char)a0, (char)a0, (char)a0
            );
            
            // 255 - alpha
            __m128i inv_alpha = _mm_sub_epi8(_mm_set1_epi8((char)255), alpha);
            
            // Unpack to 16-bit for multiplication
            __m128i src_lo = _mm_unpacklo_epi8(src_bytes, zero);
            __m128i src_hi = _mm_unpackhi_epi8(src_bytes, zero);
            __m128i dst_lo = _mm_unpacklo_epi8(dst_bytes, zero);
            __m128i dst_hi = _mm_unpackhi_epi8(dst_bytes, zero);
            __m128i alpha_lo = _mm_unpacklo_epi8(alpha, zero);
            __m128i alpha_hi = _mm_unpackhi_epi8(alpha, zero);
            __m128i inv_alpha_lo = _mm_unpacklo_epi8(inv_alpha, zero);
            __m128i inv_alpha_hi = _mm_unpackhi_epi8(inv_alpha, zero);
            
            // Compute s*a + d*(255-a) + 127
            __m128i prod_lo = _mm_add_epi16(_mm_mullo_epi16(src_lo, alpha_lo), 
                                           _mm_mullo_epi16(dst_lo, inv_alpha_lo));
            __m128i prod_hi = _mm_add_epi16(_mm_mullo_epi16(src_hi, alpha_hi), 
                                           _mm_mullo_epi16(dst_hi, inv_alpha_hi));
            prod_lo = _mm_add_epi16(prod_lo, v127);
            prod_hi = _mm_add_epi16(prod_hi, v127);
            
            // Divide by 255: (x + 1 + (x >> 8)) >> 8 is a good approximation
            // But for exact: we need (x * 0x8081) >> 23 or similar
            // Simpler exact: ((x + 1) + ((x + 1) >> 8)) >> 8
            // Let's use: (x + 128) / 255 ~ (x * 257 + 257) >> 16 for 16-bit
            // Actually for exact division by 255: result = (x * 0x8081) >> 23
            // But we need exact match, so let's do scalar for now or use lookup
            
            // For exact division by 255 in 16-bit: ((val + 1) + ((val + 1) >> 8)) >> 8
            __m128i t_lo = _mm_add_epi16(prod_lo, _mm_set1_epi16(1));
            __m128i t_hi = _mm_add_epi16(prod_hi, _mm_set1_epi16(1));
            __m128i res_lo = _mm_srli_epi16(_mm_add_epi16(t_lo, _mm_srli_epi16(t_lo, 8)), 8);
            __m128i res_hi = _mm_srli_epi16(_mm_add_epi16(t_hi, _mm_srli_epi16(t_hi, 8)), 8);
            
            // Pack back to 8-bit
            __m128i result = _mm_packus_epi16(res_lo, res_hi);
            
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out_ptr + idx), result);
        }
        
        // Handle remaining pixels
        for (; i < pixels; ++i) {
            std::size_t idx = i * 4;
            uint8_t a = src_ptr[idx + 3];
            uint32_t inv_a = 255u - a;
            
            for (int c = 0; c < 3; ++c) {
                uint32_t val = static_cast<uint32_t>(src_ptr[idx + c]) * a +
                               static_cast<uint32_t>(dst_ptr[idx + c]) * inv_a + 127u;
                out_ptr[idx + c] = static_cast<uint8_t>(val / 255u);
            }
            out_ptr[idx + 3] = 255u;
        }
        
        hash = checksum_bytes_fast(out_ptr, total_bytes);
    }
    
    return hash;
}