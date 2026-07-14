#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

alignas(64) static uint8_t blend_table[256][256];
static bool table_initialized = false;

void init_blend_table() {
    if (table_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int diff = 0; diff < 256; ++diff) {
            blend_table[a][diff] = static_cast<uint8_t>((a * diff + 127u) / 255u);
        }
    }
    table_initialized = true;
}

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    // result = (s * a + d * (255 - a) + 127) / 255
    // = d + (s - d) * a / 255 + correction
    // Using lookup table for (a * x + 127) / 255
    if (s >= d) {
        return d + blend_table[a][s - d];
    } else {
        return d - blend_table[a][d - s];
    }
}

uint64_t checksum_bytes_fast(const uint8_t* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;
    
    const uint8_t* end = data + len;
    
    // Process 8 bytes at a time
    while (data + 8 <= end) {
        hash ^= static_cast<uint64_t>(data[0]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[3]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[4]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[5]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[6]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[7]);
        hash *= prime;
        data += 8;
    }
    
    // Handle remaining bytes
    while (data < end) {
        hash ^= static_cast<uint64_t>(*data++);
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
    
    init_blend_table();
    
    const std::size_t pixels = src.size() / 4u;
    const std::size_t out_size = pixels * 4u;
    
    std::vector<uint8_t> out(out_size);
    uint8_t* out_ptr = out.data();
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* out_p = out_ptr;
        const uint8_t* src_p = src_ptr;
        const uint8_t* dst_p = dst_ptr;
        
        for (std::size_t i = 0; i < pixels; ++i) {
            const uint8_t a = src_p[3];
            
            out_p[0] = blend_byte_fast(src_p[0], dst_p[0], a);
            out_p[1] = blend_byte_fast(src_p[1], dst_p[1], a);
            out_p[2] = blend_byte_fast(src_p[2], dst_p[2], a);
            out_p[3] = 255u;
            
            src_p += 4;
            dst_p += 4;
            out_p += 4;
        }
        
        hash = checksum_bytes_fast(out_ptr, out_size);
    }
    
    return hash;
}