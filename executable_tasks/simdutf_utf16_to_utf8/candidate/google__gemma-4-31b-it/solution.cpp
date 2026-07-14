#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t compute_fnv1a(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < len; ++i) {
        hash ^= p[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // UTF-8 can take up to 3 bytes per UTF-16 code unit (for surrogate pairs)
    // Max size: 3 bytes per 2 bytes of UTF-16 (worst case for 3-byte UTF-8 chars)
    // Actually, 1 UTF-16 unit (2 bytes) -> max 3 bytes UTF-8.
    // 2 UTF-16 units (4 bytes) -> max 4 bytes UTF-8.
    // So 3 * input.size() is a safe upper bound.
    std::vector<char> buffer(input.size() * 3);
    
    // simdutf::convert_utf16le_to_utf8 expects pointers to the data.
    // u16string is guaranteed to be contiguous.
    const char16_t* src = reinterpret_cast<const char16_t*>(input.data());
    char* dst = buffer.data();
    
    // Use simdutf for high-performance transcoding
    size_t written = simdutf::convert_utf16le_to_utf8(src, input.size(), dst, buffer.size());
    
    return compute_fnv1a(dst, written);
}