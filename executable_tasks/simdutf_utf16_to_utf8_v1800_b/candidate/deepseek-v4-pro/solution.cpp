#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    // reserve worst-case output size (3x + 1 for safety)
    std::string output(input.size() * 3, '\0');
    size_t out_len = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), output.data());
    output.resize(out_len);
    
    // checksum over the exact UTF-8 bytes
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : output) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
