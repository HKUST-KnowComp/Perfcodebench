#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    // Get the required output length
    size_t utf32_len = simdutf::utf32_length_from_utf8(input.data(), input.size());
    
    // Allocate output buffer
    std::vector<char32_t> out(utf32_len);
    
    // Transcode using SIMD-accelerated conversion
    simdutf::convert_utf8_to_utf32(input.data(), input.size(), out.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf32_len; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}