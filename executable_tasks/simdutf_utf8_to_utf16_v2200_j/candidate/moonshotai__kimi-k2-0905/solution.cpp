#include "interface.h"

#include <simdutf.h>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    // Ask simdutf for the exact UTF-16 code-unit count.
    size_t expected_utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());

    // Allocate the exact buffer required (no reserve needed).
    std::unique_ptr<char16_t[]> buffer(new char16_t[expected_utf16_length]);

    // Transcode with maximum available SIMD width.
    size_t written = simdutf::convert_utf8_to_utf16le(
        input.data(), input.size(), buffer.get());

    // Compute the checksum over the resulting UTF-16 span.
    return checksum_utf16(buffer.get(), written);
}