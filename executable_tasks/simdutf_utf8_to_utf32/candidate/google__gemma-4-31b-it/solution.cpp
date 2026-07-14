#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Use simdutf to determine the required size for the output buffer
    size_t utf32_len = simdutf::utf8_to_utf32_size(reinterpret_cast<const char*>(input.data()), input.size());
    
    // Allocate a buffer for the UTF-32 result
    std::vector<char32_t> out(utf32_len);
    
    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf32(reinterpret_cast<const char*>(input.data()), input.size(), out.data());

    // Compute the checksum over the resulting UTF-32 sequence
    uint64_t hash = 1469598103934665603ULL;
    for (char32_t ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}