#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    (void)width;
    (void)height;
    const std::size_t pixels = src.size() / 4u;
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        for (std::size_t i = 0; i < pixels; ++i) {
            const std::size_t idx = i * 4u;
            const uint8_t a = src[idx + 3u];
            const uint32_t a_int = a;
            const uint32_t one_minus_a = 255u - a_int;

            // Blend R channel
            uint32_t s_r = src[idx];
            uint32_t d_r = dst[idx];
            uint32_t n_r = s_r * a_int + d_r * one_minus_a;
            uint8_t b_r = static_cast<uint8_t>((n_r + 127u) * 0x101u >> 16);
            hash ^= static_cast<uint64_t>(b_r);
            hash *= FNV_PRIME;

            // Blend G channel
            uint32_t s_g = src[idx + 1u];
            uint32_t d_g = dst[idx + 1u];
            uint32_t n_g = s_g * a_int + d_g * one_minus_a;
            uint8_t b_g = static_cast<uint8_t>((n_g + 127u) * 0x101u >> 16);
            hash ^= static_cast<uint64_t>(b_g);
            hash *= FNV_PRIME;

            // Blend B channel
            uint32_t s_b = src[idx + 2u];
            uint32_t d_b = dst[idx + 2u];
            uint32_t n_b = s_b * a_int + d_b * one_minus_a;
            uint8_t_t b_b = static_cast<uint8_t>((n_b + 127u) * 0x101u >> 16);
            hash ^= static_cast<uint64_t>(b_b);
            hash *= FNV_PRIME;

            // Add output alpha (255)
            hash ^= 255ULL;
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}