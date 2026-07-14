#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

/**
 * Computes the FNV-1a 64-bit hash over a sequence of UTF-16 code units.
 * This matches the logic of the baseline checksum_utf16 function.
 */
inline uint64_t compute_checksum(const char16_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
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

    // Calculate the required size for the UTF-16 output.
    // simdutf::utf16_length_from_utf8 is fast and handles valid UTF-8.
    size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
    
    // Use a stack buffer for small strings to avoid allocation overhead,
    // otherwise use a vector.
    if (utf16_len <= 1024) {
        char16_t buffer[1024];
        simdutf::convert_utf8_to_utf16le(input.data(), input.size(), buffer);
        return compute_checksum(buffer, utf16_len);
    } else {
        std::vector<char16_t> buffer(utf16_len);
        simdutf::convert_utf8_to_utf16le(input.data(), input.size(), buffer.data());
        return compute_checksum(buffer.data(), utf16_len);
    }
}