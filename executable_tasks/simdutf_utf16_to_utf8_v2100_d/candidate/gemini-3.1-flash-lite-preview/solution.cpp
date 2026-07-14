#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

/**
 * Optimized implementation using simdutf for high-performance transcoding.
 * The FNV-1a checksum is computed over the resulting UTF-8 buffer.
 */
uint64_t transcode_checksum(const std::u16string& input) {
    const char16_t* data = input.data();
    size_t len = input.size();

    // Calculate maximum possible UTF-8 length to allocate buffer once
    size_t max_len = simdutf::utf8_length_from_utf16(data, len);
    std::vector<char> buffer(max_len);

    // Transcode using simdutf (highly optimized SIMD implementation)
    size_t actual_len = simdutf::convert_utf16_to_utf8(data, len, buffer.data());

    // FNV-1a checksum calculation
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer.data());
    for (size_t i = 0; i < actual_len; ++i) {
        hash ^= static_cast<uint64_t>(bytes[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}