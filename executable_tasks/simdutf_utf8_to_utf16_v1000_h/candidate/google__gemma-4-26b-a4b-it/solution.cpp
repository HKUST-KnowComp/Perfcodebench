#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a style checksum for char16_t sequence
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
        return 1469598103934665603ULL;
    }

    // simdutf::convert_utf8_to_utf16 is significantly faster than manual scalar decoding.
    // It uses SIMD (AVX2, SSE4.2, or NEON) to transcode.
    // We use a vector to manage memory, but we can pre-allocate to reduce reallocations.
    // The maximum possible size for UTF-16 is input.size() (for ASCII).
    // The minimum is input.size() / 3 (for 4-byte UTF-8).
    // To avoid multiple reallocations, we use the simdutf API which handles buffer sizing.
    
    std::vector<char16_t> output;
    // simdutf::convert_utf8_to_utf16 returns a std::u16string or similar, 
    // but we can use the version that writes to a buffer or use the string return.
    // For performance and simplicity in this context, we use the string-based API.
    
    try {
        std::u16string u16_str = simdutf::convert_utf8_to_utf16(input.data(), input.size());
        return checksum_utf16(reinterpret_cast<const char16_t*>(u16_str.data()), u16_str.size());
    } catch (...) {
        // Fallback or error handling if input is invalid, though task implies valid UTF-8
        return 0;
    }
}
