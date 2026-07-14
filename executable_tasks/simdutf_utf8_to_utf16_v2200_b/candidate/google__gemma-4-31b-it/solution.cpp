#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate the required size for the UTF-16 buffer
    // simdutf::utf8_to_utf16_size returns the number of char16_t needed
    size_t utf16_size = simdutf::utf8_to_utf16_size(input.data(), input.size());
    
    // Use a vector as a buffer to ensure memory is managed and contiguous
    std::vector<char16_t> buffer(utf16_size);
    
    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf16(input.data(), input.size(), buffer.data());

    // Calculate the FNV-1a style checksum over the resulting UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (char16_t ch : buffer) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= prime;
    }
    
    return hash;
}