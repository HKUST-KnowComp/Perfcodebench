#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate the required size for the UTF-16 output buffer
    size_t utf16_len = simdutf::utf8_to_utf16_size(input.data(), input.size());
    
    // Use a vector to manage the output buffer memory
    std::vector<char16_t> out(utf16_len);
    
    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf16(input.data(), input.size(), out.data());

    // Calculate the FNV-1a style checksum over the resulting UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (char16_t ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= prime;
    }
    
    return hash;
}