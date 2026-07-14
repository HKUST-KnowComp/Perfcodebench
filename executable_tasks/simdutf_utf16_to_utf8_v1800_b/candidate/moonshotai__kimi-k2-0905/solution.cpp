#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>

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
    if (input.empty()) return checksum_bytes("", 0);

    // simdutf expects byte count, not char count
    size_t input_bytes = input.size() * sizeof(char16_t);

    // Ask simdutf for the exact UTF-8 length
    size_t utf8_len = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char*>(input.data()), input_bytes);

    // Allocate destination buffer
    std::string out;
    out.resize(utf8_len);

    // Perform the conversion
    size_t actual_len = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char*>(input.data()), input_bytes,
        out.data());

    // simdutf guarantees the returned length equals the earlier estimate
    out.resize(actual_len);

    return checksum_bytes(out.data(), out.size());
}