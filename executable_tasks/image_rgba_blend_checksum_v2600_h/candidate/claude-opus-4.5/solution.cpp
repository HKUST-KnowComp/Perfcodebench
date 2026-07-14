#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

alignas(64) uint8_t g_blend_table[256][256];
bool g_table_initialized = false;

void init_blend_table() {
    if (g_table_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int diff = 0; diff < 256; ++diff) {
            g_blend_table[a][diff] = static_cast<uint8_t>((a * diff + 127u) / 255u);
        }
    }
    g_table_initialized = true;
}

inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    // s*a + d*(255-a) = d*255 + a*(s-d) = d*255 + a*diff (with sign handling)
    // Rewritten: result = d + (s-d)*a/255
    int diff = static_cast<int>(s) - static_cast<int>(d);
    if (diff >= 0) {
        return static_cast<uint8_t>(d + g_blend_table[a][diff]);
    } else {
        return static_cast<uint8_t>(d - g_blend_table[a][-diff]);
    }
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
    
    init_blend_table();
    
    const std::size_t pixels = src.size() / 4u;
    const std::size_t total_bytes = pixels * 4u;
    
    std::vector<uint8_t> out(total_bytes);
    
    const uint8_t* __restrict src_ptr = src.data();
    const uint8_t* __restrict dst_ptr = dst.data();
    uint8_t* __restrict out_ptr = out.data();
    
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        std::size_t i = 0;
        const std::size_t pixels_4 = (pixels / 4) * 4;
        
        // Process 4 pixels at a time
        for (; i < pixels_4; i += 4) {
            const std::size_t idx0 = i * 4u;
            const std::size_t idx1 = idx0 + 4u;
            const std::size_t idx2 = idx0 + 8u;
            const std::size_t idx3 = idx0 + 12u;
            
            const uint8_t a0 = src_ptr[idx0 + 3u];
            const uint8_t a1 = src_ptr[idx1 + 3u];
            const uint8_t a2 = src_ptr[idx2 + 3u];
            const uint8_t a3 = src_ptr[idx3 + 3u];
            
            uint8_t r0 = blend_fast(src_ptr[idx0], dst_ptr[idx0], a0);
            uint8_t g0 = blend_fast(src_ptr[idx0 + 1u], dst_ptr[idx0 + 1u], a0);
            uint8_t b0 = blend_fast(src_ptr[idx0 + 2u], dst_ptr[idx0 + 2u], a0);
            
            uint8_t r1 = blend_fast(src_ptr[idx1], dst_ptr[idx1], a1);
            uint8_t g1 = blend_fast(src_ptr[idx1 + 1u], dst_ptr[idx1 + 1u], a1);
            uint8_t b1 = blend_fast(src_ptr[idx1 + 2u], dst_ptr[idx1 + 2u], a1);
            
            uint8_t r2 = blend_fast(src_ptr[idx2], dst_ptr[idx2], a2);
            uint8_t g2 = blend_fast(src_ptr[idx2 + 1u], dst_ptr[idx2 + 1u], a2);
            uint8_t b2 = blend_fast(src_ptr[idx2 + 2u], dst_ptr[idx2 + 2u], a2);
            
            uint8_t r3 = blend_fast(src_ptr[idx3], dst_ptr[idx3], a3);
            uint8_t g3 = blend_fast(src_ptr[idx3 + 1u], dst_ptr[idx3 + 1u], a3);
            uint8_t b3 = blend_fast(src_ptr[idx3 + 2u], dst_ptr[idx3 + 2u], a3);
            
            out_ptr[idx0] = r0; out_ptr[idx0 + 1u] = g0; out_ptr[idx0 + 2u] = b0; out_ptr[idx0 + 3u] = 255u;
            out_ptr[idx1] = r1; out_ptr[idx1 + 1u] = g1; out_ptr[idx1 + 2u] = b1; out_ptr[idx1 + 3u] = 255u;
            out_ptr[idx2] = r2; out_ptr[idx2 + 1u] = g2; out_ptr[idx2 + 2u] = b2; out_ptr[idx2 + 3u] = 255u;
            out_ptr[idx3] = r3; out_ptr[idx3 + 1u] = g3; out_ptr[idx3 + 2u] = b3; out_ptr[idx3 + 3u] = 255u;
            
            hash ^= r0; hash *= FNV_PRIME;
            hash ^= g0; hash *= FNV_PRIME;
            hash ^= b0; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
            
            hash ^= r1; hash *= FNV_PRIME;
            hash ^= g1; hash *= FNV_PRIME;
            hash ^= b1; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
            
            hash ^= r2; hash *= FNV_PRIME;
            hash ^= g2; hash *= FNV_PRIME;
            hash ^= b2; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
            
            hash ^= r3; hash *= FNV_PRIME;
            hash ^= g3; hash *= FNV_PRIME;
            hash ^= b3; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
        }
        
        // Handle remaining pixels
        for (; i < pixels; ++i) {
            const std::size_t idx = i * 4u;
            const uint8_t a = src_ptr[idx + 3u];
            
            uint8_t r = blend_fast(src_ptr[idx], dst_ptr[idx], a);
            uint8_t g = blend_fast(src_ptr[idx + 1u], dst_ptr[idx + 1u], a);
            uint8_t b = blend_fast(src_ptr[idx + 2u], dst_ptr[idx + 2u], a);
            
            out_ptr[idx] = r;
            out_ptr[idx + 1u] = g;
            out_ptr[idx + 2u] = b;
            out_ptr[idx + 3u] = 255u;
            
            hash ^= r; hash *= FNV_PRIME;
            hash ^= g; hash *= FNV_PRIME;
            hash ^= b; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
        }
    }
    
    return hash;
}