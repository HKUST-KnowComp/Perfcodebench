#include "interface.h"
#include <simdutf.h>
#include <string>
#include <string_view>

namespace {

// FNV-1a 64-bit hash
uint64_t checksum_bytes(std::string_view sv) {
    uint64_t hash = 14695981039346656037ULL;
    for (unsigned char ch : sv) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    // simdutf::convert_utf16le_to_utf8 returns the required buffer size when
    // given nullptr as first argument, then performs the actual conversion.
    // This avoids two passes over the data.
    size_t utf8_len = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()),
        input.size(),
        nullptr);

    std::string out;
    out.resize(utf8_len);

    simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()),
        input.size(),
        out.data());

    return checksum_bytes(out);
}