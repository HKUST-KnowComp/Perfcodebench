#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

// The original formula: (s * a + d * (255 - a) + 127) / 255
// Let x = s * a + d * (255 - a) + 127
// x / 255 is approximately (x + 1 + (x >> 8)) >> 8
// This is a standard fast integer division for 255.
inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a) + 127u;
    return static_cast<uint8_t>((val + (val >> 8) + 1) >> 8);
}

// Note: The baseline uses (val + 127) / 255. 
// To ensure EXACT correctness with the baseline's integer division:
inline uint8_t blend_exact(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a) + 127u;
    return static_cast<uint8_t>(val / 255u);
}

inline uint64_t checksum_bytes(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    const size_t total_bytes = src.size();
    const size_t pixels = total_bytes / 4u;
    
    std::vector<uint8_t> out(total_bytes);
    uint8_t* out_ptr = out.data();
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();

    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            const size_t idx = i * 4u;
            const uint8_t a = src_ptr[idx + 3u];
            
            // We must use the exact division to match the baseline's correctness rule
            // (s * a + d * (255 - a) + 127) / 255
            uint32_t inv_a = 255u - a;
            
            uint32_t r_val = static_cast<uint32_t>(src_ptr[idx]) * a + static_cast<uint32_t>(dst_ptr[idx]) * inv_a + 127u;
            out_ptr[idx] = static_cast<uint8_t>(r_val / 255u);
            
            uint32_t g_val = static_cast<uint32_t>(src_ptr[idx + 1u]) * a + static_cast<uint32_t>(dst_ptr[idx + 1u]) * inv_a + 127u;
            out_ptr[idx + 1] = static_cast<uint8_t>(g_val / 255u);
            
            uint32_t b_val = static_cast<uint32_t>(src_ptr[idx + 2u]) * a + static_cast<uint32_t>(dst_ptr[idx + 2u]) * inv_a + 127u;
            out_ptr[idx + 2] = static_cast<uint8_t>(b_val / 255u);
            
            out_ptr[idx + 3] = 255u;
        }
        last_hash = checksum_bytes(out_ptr, total_bytes);
    }

    return last_hash;
}