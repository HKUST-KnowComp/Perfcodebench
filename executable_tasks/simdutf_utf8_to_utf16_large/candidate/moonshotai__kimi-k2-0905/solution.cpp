#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
    const char* src = input.data();
    size_t src_len = input.size();

    // Ask simdutf how many UTF-16 code units we will get.
    size_t utf16_len = simdutf::utf16_length_from_utf8(src, src_len);

    // Allocate exact output buffer.
    std::u16string out;
    out.resize(utf16_len);

    // Transcode directly into it.
    simdutf::convert_utf8_to_utf16le(src, src_len,
                                     reinterpret_cast<char16_t*>(&out[0]),
                                     simdutf::utf16le);

    // Compute FNV-1a checksum over the UTF-16 sequence.
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (char16_t cu : out) {
        hash ^= static_cast<uint64_t>(cu);
        hash *= prime;
    }
    return hash;
}