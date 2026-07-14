#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

static thread_local std::vector<char16_t> buffer;

uint64_t transcode_checksum(const std::string& input) {
    // Maximum possible output length: each byte can become a surrogate pair (2 units).
    size_t max_units = input.size() * 2;
    if (buffer.capacity() < max_units) {
        buffer.reserve(max_units);
    }
    buffer.resize(max_units);

    size_t output_units = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), buffer.data());

    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < output_units; ++i) {
        hash ^= static_cast<uint64_t>(buffer[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
