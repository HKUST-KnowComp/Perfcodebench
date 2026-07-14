#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    // UTF-8 to UTF-16LE never produces more code units than input bytes.
    // (Max 4 UTF-8 bytes -> 2 UTF-16 code units, so code_units <= bytes)
    std::vector<char16_t> out(input.size());
    size_t utf16_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
    
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf16_len; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
