#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

alignas(64) uint8_t g_blend_table[256][256];
bool g_table_initialized = false;

void init_blend_table() {
    if (g_table_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int s = 0; s < 256; ++s) {
            g_blend_table[a][s] = static_cast<uint8_t>((s * a + 127u) / 255u);
        }
    }
    g_table_initialized = true;
}

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    return g_blend_table[a][s] + g_blend_table[255 - a][d];
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
    const std::size_t out_size = pixels * 4u;
    
    std::vector<uint8_t> out(out_size);
    
    const uint8_t* __restrict src_ptr = src.data();
    const uint8_t* __restrict dst_ptr = dst.data();
    uint8_t* __restrict out_ptr = out.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::size_t i = 0;
        const std::size_t pixels_4 = pixels & ~3ULL;
        
        for (; i < pixels_4; i += 4) {
            const std::size_t idx0 = i * 4u;
            const std::size_t idx1 = idx0 + 4u;
            const std::size_t idx2 = idx0 + 8u;
            const std::size_t idx3 = idx0 + 12u;
            
            const uint8_t a0 = src_ptr[idx0 + 3u];
            const uint8_t a1 = src_ptr[idx1 + 3u];
            const uint8_t a2 = src_ptr[idx2 + 3u];
            const uint8_t a3 = src_ptr[idx3 + 3u];
            
            out_ptr[idx0]     = blend_byte_fast(src_ptr[idx0],     dst_ptr[idx0],     a0);
            out_ptr[idx0 + 1] = blend_byte_fast(src_ptr[idx0 + 1], dst_ptr[idx0 + 1], a0);
            out_ptr[idx0 + 2] = blend_byte_fast(src_ptr[idx0 + 2], dst_ptr[idx0 + 2], a0);
            out_ptr[idx0 + 3] = 255u;
            
            out_ptr[idx1]     = blend_byte_fast(src_ptr[idx1],     dst_ptr[idx1],     a1);
            out_ptr[idx1 + 1] = blend_byte_fast(src_ptr[idx1 + 1], dst_ptr[idx1 + 1], a1);
            out_ptr[idx1 + 2] = blend_byte_fast(src_ptr[idx1 + 2], dst_ptr[idx1 + 2], a1);
            out_ptr[idx1 + 3] = 255u;
            
            out_ptr[idx2]     = blend_byte_fast(src_ptr[idx2],     dst_ptr[idx2],     a2);
            out_ptr[idx2 + 1] = blend_byte_fast(src_ptr[idx2 + 1], dst_ptr[idx2 + 1], a2);
            out_ptr[idx2 + 2] = blend_byte_fast(src_ptr[idx2 + 2], dst_ptr[idx2 + 2], a2);
            out_ptr[idx2 + 3] = 255u;
            
            out_ptr[idx3]     = blend_byte_fast(src_ptr[idx3],     dst_ptr[idx3],     a3);
            out_ptr[idx3 + 1] = blend_byte_fast(src_ptr[idx3 + 1], dst_ptr[idx3 + 1], a3);
            out_ptr[idx3 + 2] = blend_byte_fast(src_ptr[idx3 + 2], dst_ptr[idx3 + 2], a3);
            out_ptr[idx3 + 3] = 255u;
        }
        
        for (; i < pixels; ++i) {
            const std::size_t idx = i * 4u;
            const uint8_t a = src_ptr[idx + 3u];
            out_ptr[idx]     = blend_byte_fast(src_ptr[idx],     dst_ptr[idx],     a);
            out_ptr[idx + 1] = blend_byte_fast(src_ptr[idx + 1], dst_ptr[idx + 1], a);
            out_ptr[idx + 2] = blend_byte_fast(src_ptr[idx + 2], dst_ptr[idx + 2], a);
            out_ptr[idx + 3] = 255u;
        }
        
        // FNV-1a hash with unrolling
        hash = 1469598103934665603ULL;
        const uint8_t* hp = out_ptr;
        const uint8_t* end = out_ptr + out_size;
        const uint8_t* end8 = out_ptr + (out_size & ~7ULL);
        
        while (hp < end8) {
            hash ^= static_cast<uint64_t>(hp[0]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[1]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[2]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[3]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[4]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[5]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[6]);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(hp[7]);
            hash *= 1099511628211ULL;
            hp += 8;
        }
        
        while (hp < end) {
            hash ^= static_cast<uint64_t>(*hp++);
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}