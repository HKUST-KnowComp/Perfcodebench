#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // UTF-8 can be up to 3 bytes per UTF-16 code unit (for surrogate pairs)
    // Max size: 3 bytes per 2-byte unit is a safe upper bound for most cases,
    // but the absolute max is 3 bytes per 16-bit unit if every unit is a lead surrogate
    // (though invalid). For valid UTF-16, max is 3 bytes per 2-byte unit (4 bytes for 2 units).
    std::vector<char> buffer(input.size() * 3);
    
    // simdutf::utf16le_to_utf8 expects a pointer to the UTF-16LE data.
    // std::u16string is typically UTF-16. On little-endian systems, it is UTF-16LE.
    const char16_t* src = reinterpret_cast<const char16_t*>(input.data());
    char* dst = buffer.data();
    
    // Use simdutf for high-performance transcoding
    simdutf::utf16le_to_utf8(src, input.size(), dst, buffer.size());
    
    // The function returns the number of bytes written to the destination
    size_t written = simdutf::utf16le_to_utf8(src, input.size(), dst, buffer.size());
    
    return checksum_bytes(dst, written);
}
