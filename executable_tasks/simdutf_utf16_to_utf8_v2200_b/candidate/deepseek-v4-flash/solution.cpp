#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    // Compute exact UTF-8 length
    size_t utf8_length = simdutf::utf8_length_from_utf16(input.data(), input.size());
    std::string out(utf8_length, '\0');
    // Convert UTF-16LE to UTF-8
    size_t written = simdutf::convert_utf16_to_utf8(input.data(), input.size(), &out[0]);
    // (written is unused but equals utf8_length for valid input)
    // Compute FNV-1a checksum over the UTF-8 bytes
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}