#include "interface.h"
#include <simdutf.h>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    size_t utf8len = simdutf::utf16le_length_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
    std::string out;
    out.resize(utf8len);
    simdutf::convert_utf16le_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), out.data());
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
