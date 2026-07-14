#include "interface.h"
#include <cstdint>
#include <string>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
    size_t input_len = input.size();
    if (input_len == 0) {
        return 1469598103934665603ULL;
    }
    // Maximum possible UTF-8 output length for given number of UTF-16 code units
    size_t max_utf8_len = simdutf::maximal_binary_length_of_utf8_from_utf16(input_len);
    std::string output(max_utf8_len, '\0');
    size_t written = simdutf::convert_utf16le_to_utf8(input.data(), input_len, &output[0]);
    // Compute FNV-1a hash over the exact written bytes
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(output.data());
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
