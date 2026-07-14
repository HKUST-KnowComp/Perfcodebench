#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    const char16_t* src = reinterpret_cast<const char16_t*>(input.data());
    size_t src_len = input.size();

    // Calculate required size for the UTF-8 output
    size_t utf8_len = simdutf::utf16le_to_utf8_size(src, src_len);
    
    // Use a vector as a buffer to avoid std::string overhead
    std::vector<char> buffer(utf8_len);
    
    // Perform the high-performance SIMD conversion
    simdutf::convert_utf16le_to_utf8(src, src_len, buffer.data(), utf8_len);

    // Compute FNV-1a checksum over the resulting bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    for (size_t i = 0; i < utf8_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}