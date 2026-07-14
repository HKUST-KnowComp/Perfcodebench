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

    // Pre-allocate buffer. In the worst case (all ASCII), UTF-16 size is input.size().
    // In the case of 4-byte UTF-8, UTF-16 uses 2 units (4 bytes), so size remains similar.
    // We use a vector of char16_t for automatic memory management.
    std::vector<char16_t> out(input.size());
    size_t utf16_len = 0;

    // Use simdutf for high-performance SIMD-accelerated transcoding
    // simdutf::convert_utf8_to_utf16 returns the number of char16_t elements written
    utf16_len = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());

    return checksum_utf16(out.data(), utf16_len);
}