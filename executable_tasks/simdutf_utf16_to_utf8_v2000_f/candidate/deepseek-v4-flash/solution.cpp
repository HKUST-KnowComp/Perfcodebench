#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    size_t max_output = input.size() * 3;
    std::string output(max_output, '\0');
    size_t written = simdutf::convert_utf16_to_utf8(input.data(), input.size(), &output[0]);
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(output.data());
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}