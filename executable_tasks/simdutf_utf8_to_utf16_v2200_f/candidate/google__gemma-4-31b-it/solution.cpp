#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // The maximum number of UTF-16 code units is the number of UTF-8 bytes
    // (for ASCII) or slightly more in extreme cases, but for valid UTF-8,
    // the number of UTF-16 units is at most input.size().
    // For characters > U+FFFF, 1 UTF-8 sequence (4 bytes) becomes 2 UTF-16 units.
    // For ASCII, 1 byte becomes 1 unit. 
    // The worst case for size is ASCII: input.size() units.
    std::vector<char16_t> buffer(input.size() + 1);
    
    // simdutf::convert_utf8_to_utf16 returns the number of UTF-16 code units written.
    // It is highly optimized using SIMD.
    size_t written = simdutf::convert_utf8_to_utf16(
        reinterpret_cast<const char*>(input.data()), 
        input.size(), 
        buffer.data()
    );

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(buffer[i]);
        hash *= prime;
    }

    return hash;
}