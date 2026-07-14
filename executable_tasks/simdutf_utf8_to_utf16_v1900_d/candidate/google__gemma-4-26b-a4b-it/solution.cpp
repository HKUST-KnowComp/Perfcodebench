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

    // simdutf::convert_utf8_to_utf16 is highly optimized using SIMD instructions.
    // It returns a std::u16string which manages its own memory.
    // We use the simdutf library to perform the heavy lifting.
    std::u16string output = simdutf::convert_utf8_to_utf16(input.data(), input.size());
    
    return checksum_utf16(reinterpret_cast<const char16_t*>(output.data()), output.size());
}
