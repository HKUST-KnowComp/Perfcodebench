#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a 64-bit checksum implementation
inline uint64_t calculate_fnv1a(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data.data());
    size_t len = data.size();
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Use simdutf for high-performance transcoding
    // simdutf::convert_utf16_to_utf8 returns a std::string
    // It handles surrogate pairs and validation internally.
    std::string utf8_output = simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());

    return calculate_fnv1a(utf8_output);
}
