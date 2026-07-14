#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    size_t utf8_len = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char16_t*>(input.data()), input.size());
    std::string out(utf8_len, '\0');
    simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), input.size(),
        &out[0]);
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
    size_t n = out.size();
    for (size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
