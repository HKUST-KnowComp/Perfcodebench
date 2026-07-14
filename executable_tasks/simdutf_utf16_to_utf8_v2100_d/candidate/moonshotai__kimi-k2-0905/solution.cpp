#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>
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

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    const char16_t* src = input.data();
    size_t src_len = input.size();
    size_t dst_len = simdutf::utf8_length_from_utf16le(src, src_len);

    std::string dst;
    dst.resize(dst_len);

    size_t written = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const simdutf::char16_t*>(src), src_len, dst.data());
    dst.resize(written);

    return checksum_bytes(dst);
}

// simdutf::convert_utf16le_to_utf8 returns number of bytes written.
// utf8_length_from_utf16le gives exact upper bound of bytes needed, so
// std::string is sized once; resize is cheap if sizes differ.
// Correctness fully preserved: utf8 bytes and their order are unchanged.
// Checksum on those bytes is therefore unchanged.