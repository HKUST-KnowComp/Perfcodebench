#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    size_t expected_utf16_words = simdutf::utf16_length_from_utf8(input.data(), input.size());
    std::u16string out;
    out.resize(expected_utf16_words);
    size_t actual_words = simdutf::convert_utf8_to_utf16le(input.data(), input.size(),
                                                           reinterpret_cast<char16_t*>(out.data()));
    out.resize(actual_words);
    return checksum_utf16(out.data(), out.size());
}