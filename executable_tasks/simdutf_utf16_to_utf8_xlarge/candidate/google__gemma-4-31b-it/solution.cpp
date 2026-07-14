#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate the required size for the UTF-8 output
    size_t utf8_size = simdutf::utf16le_to_utf8_size(input.data(), input.size());
    
    // Use a vector to avoid std::string's potential zero-termination overhead
    // and ensure a contiguous buffer for the checksum.
    std::vector<uint8_t> buffer(utf8_size);
    
    // Perform the high-performance SIMD conversion
    simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.data(), utf8_size);

    // Compute the FNV-1a style checksum over the resulting bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    size_t len = buffer.size();
    
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}