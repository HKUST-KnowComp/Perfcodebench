#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    // Maximum possible size for UTF-8 is 3 bytes per UTF-16 code unit
    size_t max_len = simdutf::utf8_length_from_utf16(input.size());
    std::vector<char> buffer(max_len);

    // Perform transcoding using simdutf optimized routines
    size_t actual_len = simdutf::convert_utf16_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        buffer.data()
    );

    // FNV-1a checksum calculation
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    for (size_t i = 0; i < actual_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}