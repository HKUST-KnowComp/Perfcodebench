#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum for char16_t sequence
inline uint64_t checksum_utf16_fast(const char16_t* data, size_t len) {
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
        return checksum_utf16_fast(nullptr, 0);
    }

    // Use simdutf for high-performance transcoding
    // We need to determine the size of the output buffer first or use a container.
    // simdutf::convert_utf8_to_utf16 provides a fast path.
    
    size_t utf16_len = simdutf::utf8_to_utf16_length(input.data(), input.size());
    std::vector<char16_t> output(utf16_len);
    
    simdutf::convert_utf8_to_utf16(input.data(), input.size(), output.data());

    return checksum_utf16_fast(output.data(), output.size());
}
