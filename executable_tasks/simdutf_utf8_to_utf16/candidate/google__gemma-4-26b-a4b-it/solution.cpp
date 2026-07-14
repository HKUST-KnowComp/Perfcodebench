#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a style checksum for char16_t
inline uint64_t checksum_utf16_optimized(const char16_t* data, size_t len) {
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
        return checksum_utf16_optimized(nullptr, 0);
    }

    // simdutf::convert_utf8_to_utf16 requires a destination buffer.
    // We estimate the maximum possible size (UTF-16 units <= UTF-8 bytes).
    // For a valid UTF-8 string, the number of char16_t units is at most input.size().
    // However, surrogate pairs mean we need to be careful. 
    // A safe upper bound for char16_t count is input.size().
    std::vector<char16_t> output(input.size());
    
    size_t utf16_len = simdutf::convert_utf8_to_utf16(
        input.data(), 
        input.size(), 
        output.data()
    );

    return checksum_utf16_optimized(output.data(), utf16_len);
}
