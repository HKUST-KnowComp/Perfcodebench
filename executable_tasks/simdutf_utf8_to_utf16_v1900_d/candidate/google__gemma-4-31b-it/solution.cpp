#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate the required size for the UTF-16 buffer
    size_t utf16_size = simdutf::utf8_to_utf16_size(input.data(), input.size());
    
    // Use a vector to manage memory for the output
    std::vector<char16_t> out(utf16_size);
    
    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf16(input.data(), input.size(), out.data());

    // Compute the FNV-1a style checksum over the resulting UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < utf16_size; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= prime;
    }
    
    return hash;
}