#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
    // Calculate required output size
    size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
    
    // Allocate output buffer
    std::vector<char16_t> out(utf16_len);
    
    // Perform SIMD-accelerated transcoding
    simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf16_len; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}