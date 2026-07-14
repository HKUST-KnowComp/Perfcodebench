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

    // simdutf::convert_utf8_to_utf16 requires a destination buffer.
    // We first calculate the required size or use a reasonable estimate.
    // For performance in a large workload, we use simdutf's optimized conversion.
    
    size_t utf16_len = simdutf::utf8_to_utf16_length(input.data(), input.size());
    std::vector<char16_t> out(utf16_len);
    
    // Perform SIMD accelerated transcoding
    simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());

    return checksum_utf16(out.data(), out.size());
}
