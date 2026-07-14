#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <iostream>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate required size for UTF-16 output
    // simdutf::utf8_to_utf16_size returns the number of char16_t needed
    size_t utf16_size = simdutf::utf8_to_utf16_size(input.data(), input.size());
    
    // Use a buffer to hold the transcoded result
    std::vector<char16_t> buffer(utf16_size);
    
    // Perform the SIMD-accelerated transcoding
    simdutf::utf8_to_utf16(input.data(), input.size(), buffer.data());

    // Calculate the checksum over the resulting UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    
    for (size_t i = 0; i < utf16_size; ++i) {
        hash ^= static_cast<uint64_t>(buffer[i]);
        hash *= multiplier;
    }
    
    return hash;
}