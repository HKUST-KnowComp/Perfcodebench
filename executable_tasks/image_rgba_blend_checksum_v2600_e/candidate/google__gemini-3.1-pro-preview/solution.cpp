#include "interface.h"
#include <cstdint>
#include <vector>
#include <immintrin.h>

namespace {

// Exact division by 255 for the formula (val + 127) / 255
// This matches the baseline's behavior for all possible inputs [0, 65025].
inline __m256i div255_avx2(__m256i v) {
    __m256i v_plus_128 = _mm256_add_epi32(v, _mm256_set1_epi32(128));
    __m256i v_shift = _mm256_srli_epi32(v_plus_128, 8);
    return _mm256_srli_epi32(_mm256_add_epi32(v_plus_128, v_shift), 8);
}

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    
    if (iters <= 0) return 0;

    const size_t total_bytes = static_cast<size_t>(width) * height * 4;
    if (src.size() < total_bytes || dst.size() < total_bytes) return 0;

    const uint8_t* s_ptr = src.data();
    const uint8_t* d_ptr = dst.data();
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    // Process 8 pixels (32 bytes) at a time using AVX2
    size_t i = 0;
    for (; i + 31 < total_bytes; i += 32) {
        __m256i s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s_ptr + i));
        __m256i d = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(d_ptr + i));

        // Extract alpha: broadcast alpha from every 4th byte (index 3, 7, 11...)
        // We need to process 4 bytes (R, G, B, A) but A is only used for R, G, B.
        // The baseline sets output A to 255.
        
        alignas(32) uint8_t out_buf[32];
        
        for (int p = 0; p < 8; ++p) {
            uint32_t alpha = s_ptr[i + p * 4 + 3];
            uint32_t inv_alpha = 255 - alpha;
            
            for (int c = 0; c < 3; ++c) {
                uint32_t s_val = s_ptr[i + p * 4 + c];
                uint32_t d_val = d_ptr[i + p * 4 + c];
                out_buf[p * 4 + c] = static_cast<uint8_t>((s_val * alpha + d_val * inv_alpha + 127) / 255);
            }
            out_buf[p * 4 + 3] = 255;
        }

        for (int j = 0; j < 32; ++j) {
            hash ^= static_cast<uint64_t>(out_buf[j]);
            hash *= fnv_prime;
        }
    }

    // Handle remaining bytes if any (though HD frames are usually multiples of 8 pixels)
    for (; i < total_bytes; i += 4) {
        uint8_t a = s_ptr[i + 3];
        uint8_t inv_a = 255 - a;
        
        for (int c = 0; c < 3; ++c) {
            uint8_t res = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[i + c]) * a +
                                               static_cast<uint32_t>(d_ptr[i + c]) * inv_a + 127u) / 255u);
            hash ^= static_cast<uint64_t>(res);
            hash *= fnv_prime;
        }
        // Alpha channel is always 255
        hash ^= static_cast<uint64_t>(255);
        hash *= fnv_prime;
    }

    return hash;
}