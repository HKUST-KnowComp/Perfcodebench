#include "interface.h"
#include <simdutf.h>
#include <memory>
#include <cstddef>

uint64_t transcode_checksum(const std::string& input) {
    // Use simdutf's fast, unsafe (valid input) converter
    auto buffer = std::make_unique<char16_t[]>(input.size());
    size_t out_len = simdutf::convert_valid_utf8_to_utf16le(input.data(), input.size(), buffer.get());

    // Compute checksum over the output code units
    uint64_t hash = 1469598103934665603ULL;
    const char16_t* p = buffer.get();
    const char16_t* endp = p + out_len;
    while (p < endp) {
        hash ^= static_cast<uint64_t>(*p);
        hash *= 1099511628211ULL;
        ++p;
    }
    return hash;
}