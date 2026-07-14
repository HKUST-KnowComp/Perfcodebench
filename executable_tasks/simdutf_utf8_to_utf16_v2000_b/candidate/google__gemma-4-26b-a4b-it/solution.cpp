#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a style checksum for char16_t sequence
uint64_t checksum_utf16(const std::vector<char16_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    const char16_t* ptr = data.data();
    const size_t size = data.size();
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_utf16({});
    }

    // Pre-allocate maximum possible size for UTF-16 (1 byte UTF-8 -> 1 char16_t)
    // Note: UTF-8 to UTF-16 can result in fewer or equal number of code units
    // except for surrogate pairs, but the total number of char16_t units 
    // will never exceed the number of bytes in the UTF-8 string for valid UTF-8.
    std::vector<char16_t> out(input.size());
    size_t utf16_len = 0;

    // Use simdutf for high-performance SIMD transcoding
    // simdutf::convert_utf8_to_utf16 returns the number of char16_t written
    utf16_len = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());

    // Resize to actual length to ensure checksum is correct
    out.resize(utf16_len);

    return checksum_utf16(out);
}