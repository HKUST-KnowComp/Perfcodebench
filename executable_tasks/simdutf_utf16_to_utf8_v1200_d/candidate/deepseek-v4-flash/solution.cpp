#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) return 1469598103934665603ULL;
    size_t utf8len = simdutf::utf16_to_utf8_length(input.data(), input.size());
    std::string out(utf8len, '\0');
    simdutf::convert_utf16le_to_utf8(input.data(), input.size(), &out[0]);
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
