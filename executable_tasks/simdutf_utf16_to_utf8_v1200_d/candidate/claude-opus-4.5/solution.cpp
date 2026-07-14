#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    // Calculate required UTF-8 length
    size_t utf8_len = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char16_t*>(input.data()), input.size());
    
    // Allocate buffer for UTF-8 output
    std::vector<char> utf8_buf(utf8_len);
    
    // Perform SIMD-accelerated transcoding
    size_t written = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), input.size(),
        utf8_buf.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(utf8_buf.data());
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}