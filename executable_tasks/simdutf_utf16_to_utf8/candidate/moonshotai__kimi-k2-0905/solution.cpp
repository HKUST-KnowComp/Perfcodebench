#include "interface.h"
#include <simdutf.h>
#include <string>

namespace {

uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    // simdutf expects length in bytes, not in char16_t units
    size_t utf16_bytes = input.size() * sizeof(char16_t);

    // Ask simdutf for the exact number of UTF-8 bytes required
    size_t utf8_bytes = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char*>(input.data()), utf16_bytes);

    std::string out;
    out.resize(utf8_bytes);

    // Perform the actual conversion
    size_t actual_bytes = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char*>(input.data()), utf16_bytes,
        out.data());

    out.resize(actual_bytes); // shrink to exact size (usually a no-op)
    return checksum_bytes(out);
}