#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    // Allocate output buffer large enough for the worst case: each input byte could become one UTF-16 code unit (ASCII).
    std::vector<char16_t> output(input.size());
    
    // Convert the valid UTF-8 string to UTF-16LE using the SIMD-accelerated library.
    size_t code_units = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), output.data());
    
    // Compute the checksum over the exact UTF-16 code unit sequence.
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < code_units; ++i) {
        hash ^= static_cast<uint64_t>(output[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}