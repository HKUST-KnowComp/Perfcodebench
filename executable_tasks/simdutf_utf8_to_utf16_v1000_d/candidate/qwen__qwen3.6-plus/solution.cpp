#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // UTF-16 code units count is always <= UTF-8 byte count.
    std::vector<char16_t> out(input.size());
    size_t len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
    
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}