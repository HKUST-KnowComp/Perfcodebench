#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    size_t max_utf16 = simdutf::utf16_length_from_utf8(input.data(), input.size());
    std::vector<char16_t> out(max_utf16);
    size_t utf16_len = simdutf::convert_utf8_to_utf16le(
        input.data(), input.size(), reinterpret_cast<char16_t*>(out.data()));
    out.resize(utf16_len);

    uint64_t hash = 14695981039346656037ULL;
    for (char16_t ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}