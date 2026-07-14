#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::u16string& input) {
    const char16_t* data = input.data();
    size_t length = input.size();
    // Maximum UTF-8 length: 3 bytes per UTF-16 code unit
    std::vector<char> buffer(length * 3);
    size_t utf8_len = simdutf::convert_utf16_to_utf8(data, length, buffer.data());
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf8_len; ++i) {
        hash ^= static_cast<unsigned char>(buffer[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
