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

    // Calculate the checksum over the resulting UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    for (char16_t ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}