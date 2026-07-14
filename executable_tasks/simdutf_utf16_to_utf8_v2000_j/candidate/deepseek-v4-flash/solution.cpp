#include "interface.h"
#include <cstdint>
#include <string>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
    // Compute exact UTF-8 output length
    size_t utf8_length = simdutf::utf8_length_from_utf16(input.data(), input.size());
    // Allocate buffer
    std::string out(utf8_length, '\0');
    // Convert UTF-16 to UTF-8
    size_t written = simdutf::convert_utf16_to_utf8(input.data(), input.size(), out.data());
    // Compute FNV-1a hash over the UTF-8 bytes
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
