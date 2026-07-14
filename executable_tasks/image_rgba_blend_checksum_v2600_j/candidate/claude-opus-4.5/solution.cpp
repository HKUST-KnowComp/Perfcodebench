#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

alignas(64) uint8_t blend_table[256][256];

void init_blend_table() {
    static bool initialized = false;
    if (initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int s = 0; s < 256; ++s) {
            blend_table[a][s] = static_cast<uint8_t>((s * a + 127u) / 255u);
        }
    }
    initialized = true;
}

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    return blend_table[a][s] + blend_table[255 - a][d];
}

uint64_t checksum_bytes_fast(const uint8_t* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;
    
    const uint8_t* end = data + len;
    
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
        uint8_t* o = out_ptr;
        const uint8_t* s = src_ptr;
        const uint8_t* d = dst_ptr;
        
        for (std::size_t i = 0; i < pixels; ++i) {
            const uint8_t a = s[3];
            o[0] = blend_byte_fast(s[0], d[0], a);
            o[1] = blend_byte_fast(s[1], d[1], a);
            o[2] = blend_byte_fast(s[2], d[2], a);
            o[3] = 255u;
            o += 4;
            s += 4;
            d += 4;
        }
        
        hash = checksum_bytes_fast(out_ptr, out_size);
    }
    
    return hash;
}