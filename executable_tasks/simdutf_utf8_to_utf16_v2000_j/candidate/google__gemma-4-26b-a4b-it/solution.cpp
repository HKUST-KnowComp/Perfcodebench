#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a style checksum for char16_t
inline uint64_t checksum_utf16(const char16_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_utf16(nullptr, 0);
    }

    // simdutf::convert_utf8_to_utf16 is highly optimized using SIMD instructions.
    // We use a vector to manage the lifetime of the output buffer.
    // The size of UTF-16 output is at most the size of UTF-8 input.
    std::vector<char16_t> output(input.size());
    
    size_t utf16_len = simdutf::convert_utf8_to_utf16(
        input.data(), 
        input.size(), 
        output.data()
    );

    return checksum_utf16(output.data(), utf16_len);
}
