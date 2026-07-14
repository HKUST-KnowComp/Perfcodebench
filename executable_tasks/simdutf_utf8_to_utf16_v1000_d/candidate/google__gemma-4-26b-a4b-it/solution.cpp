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
        return 1469598103934665603ULL;
    }

    // Pre-allocate buffer. UTF-16 size is at most input.size() (for ASCII) 
    // or input.size()/2 (for 4-byte UTF-8). 
    // Using input.size() as a safe upper bound for char16_t count.
    std::vector<char16_t> out(input.size());
    
    // Use simdutf for high-performance SIMD transcoding
    size_t utf16_len = simdutf::utf8_to_utf16(input.data(), input.size(), out.data());
    
    // Resize vector to actual length produced
    out.resize(utf16_len);

    return checksum_utf16(out.data(), out.size());
}
