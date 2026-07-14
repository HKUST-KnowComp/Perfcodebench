#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

// Precomputed blend table: blend_table[a][s] = (s * a + 127) / 255
// We'll compute d_component as blend_table[a][s] + blend_table[255-a][d]
alignas(64) uint8_t blend_table[256][256];
alignas(64) uint8_t inv_blend_table[256][256];
bool tables_initialized = false;

void init_tables() {
    if (tables_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int v = 0; v < 256; ++v) {
            // (v * a + 127) / 255
            blend_table[a][v] = static_cast<uint8_t>((v * a + 127) / 255);
            inv_blend_table[a][v] = static_cast<uint8_t>((v * (255 - a) + 127) / 255);
        }
    }
    tables_initialized = true;
}

inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    return blend_table[a][s] + inv_blend_table[a][d];
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
    
    init_tables();
    
    const std::size_t total_bytes = src.size();
    const std::size_t pixels = total_bytes / 4u;
    
    // Pre-allocate output buffer once
    std::vector<uint8_t> out(total_bytes);
    
    const uint8_t* __restrict src_ptr = src.data();
    const uint8_t* __restrict dst_ptr = dst.data();
    uint8_t* __restrict out_ptr = out.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Blend all pixels
        for (std::size_t i = 0; i < pixels; ++i) {
            const std::size_t idx = i * 4u;
            const uint8_t a = src_ptr[idx + 3u];
            
            out_ptr[idx]     = blend_fast(src_ptr[idx],     dst_ptr[idx],     a);
            out_ptr[idx + 1] = blend_fast(src_ptr[idx + 1], dst_ptr[idx + 1], a);
            out_ptr[idx + 2] = blend_fast(src_ptr[idx + 2], dst_ptr[idx + 2], a);
            out_ptr[idx + 3] = 255u;
        }
        
        // FNV-1a checksum - unrolled for better performance
        uint64_t h = 1469598103934665603ULL;
        const uint64_t prime = 1099511628211ULL;
        
        std::size_t j = 0;
        const std::size_t unroll_limit = total_bytes & ~7ULL;
        
        for (; j < unroll_limit; j += 8) {
            h ^= static_cast<uint64_t>(out_ptr[j]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 1]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 2]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 3]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 4]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 5]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 6]);
            h *= prime;
            h ^= static_cast<uint64_t>(out_ptr[j + 7]);
            h *= prime;
        }
        
        for (; j < total_bytes; ++j) {
            h ^= static_cast<uint64_t>(out_ptr[j]);
            h *= prime;
        }
        
        hash = h;
    }
    
    return hash;
}