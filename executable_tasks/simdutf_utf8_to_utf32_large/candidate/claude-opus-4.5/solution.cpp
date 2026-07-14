#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    // Get the length of UTF-32 output
    size_t utf32_len = simdutf::utf32_length_from_utf8(input.data(), input.size());
    
    // Allocate buffer for UTF-32 output
    std::vector<char32_t> out(utf32_len);
    
    // Transcode using simdutf (SIMD-accelerated)
    simdutf::convert_utf8_to_utf32(input.data(), input.size(), out.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    const char32_t* data = out.data();
    size_t len = out.size();
    
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}