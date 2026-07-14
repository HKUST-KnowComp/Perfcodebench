#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(bytes[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate required size for UTF-8 output
    size_t utf8_len = simdutf::convert_utf16le_to_utf8_size(input.data(), input.size());
    
    // Use a vector to avoid std::string overhead and ensure contiguous memory
    std::vector<char> buffer(utf8_len);
    
    // Perform the high-performance SIMD conversion
    simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.data(), utf8_len);
    
    return checksum_bytes(buffer.data(), utf8_len);
}