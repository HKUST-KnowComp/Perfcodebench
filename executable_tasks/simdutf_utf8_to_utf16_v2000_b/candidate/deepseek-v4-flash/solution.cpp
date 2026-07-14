#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
    // Allocate enough space for UTF-16 output (max expansion ratio is 2)
    std::vector<uint16_t> output(input.size() * 2 + 2);
    size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), output.data());
    
    // FNV-1a 64-bit hash over the UTF-16 code units
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(output[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
