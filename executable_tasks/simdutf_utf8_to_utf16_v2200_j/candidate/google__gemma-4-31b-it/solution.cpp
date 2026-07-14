#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate required size for UTF-16 output
    size_t utf16_len = simdutf::utf8_to_utf16_size(input.data(), input.size());
    
    // Use a vector as a buffer for the transcoded data
    std::vector<char16_t> buffer(utf16_len);
    
    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf16(input.data(), input.size(), buffer.data());

    // Calculate the FNV-1a style checksum over the resulting UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < utf16_len; ++i) {
        hash ^= static_cast<uint64_t>(buffer[i]);
        hash *= prime;
    }
    
    return hash;
}