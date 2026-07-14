#include "interface.h"
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
    // Use simdutf's fast UTF-16LE to UTF-8 converter
    std::string utf8 = simdutf::convert_utf16_to_utf8(input.data(), input.size());
    // Compute the same checksum as the baseline
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : utf8) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
