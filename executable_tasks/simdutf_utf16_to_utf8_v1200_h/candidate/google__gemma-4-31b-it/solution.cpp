#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate required size for UTF-8 output
    size_t utf8_size = simdutf::utf16_to_utf8_size(reinterpret_cast<const char16_t*>(input.data()), input.size());
    
    // Use a vector for the buffer to avoid std::string overhead and ensure contiguous memory
    std::vector<char> buffer(utf8_size);
    
    // Perform the SIMD-accelerated transcoding
    simdutf::utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data());

    // Compute FNV-1a checksum over the resulting bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    for (size_t i = 0; i < utf8_size; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }

    return hash;
}