#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    // simdutf::utf16_to_utf8_length provides the exact buffer size needed
    size_t utf8_len = simdutf::utf16_to_utf8_length(input.data(), input.size());
    std::vector<char> buffer(utf8_len);
    
    // Perform the transcoding using simdutf's optimized implementation
    size_t actual_len = simdutf::convert_utf16_to_utf8(input.data(), input.size(), buffer.data());
    
    // FNV-1a checksum calculation
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    for (size_t i = 0; i < actual_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}