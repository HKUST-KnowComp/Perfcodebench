#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

uint64_t compute_checksum(const char16_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate the required size for the UTF-16 buffer
    size_t utf16_len = simdutf::utf8_to_utf16_size(input.data(), input.size());
    
    // Use a vector as a managed buffer for the result
    std::vector<char16_t> out(utf16_len);
    
    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf16(input.data(), input.size(), out.data());
    
    return compute_checksum(out.data(), utf16_len);
}