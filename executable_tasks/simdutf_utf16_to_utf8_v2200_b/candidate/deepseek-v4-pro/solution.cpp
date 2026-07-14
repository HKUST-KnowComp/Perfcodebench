#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    std::string out;
    out.resize(input.size() * 3);
    const char* src = reinterpret_cast<const char*>(input.data());
    size_t utf8_len = simdutf::convert_utf16le_to_utf8(src, input.size() * sizeof(char16_t), out.data());
    out.resize(utf8_len);

    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
    const unsigned char* end = p + utf8_len;
    while (p < end) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= 1099511628211ULL;
    }
    return hash;
}