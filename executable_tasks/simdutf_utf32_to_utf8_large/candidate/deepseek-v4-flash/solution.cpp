#include "interface.h"

#include <simdutf.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    const size_t len = input.size();
    if (len == 0) {
        // empty input yields empty UTF-8, checksum of empty string
        uint64_t hash = 1469598103934665603ULL;
        return hash;
    }
    // Compute required UTF-8 length
    size_t utf8_len = simdutf::utf32_length_to_utf8(input.data(), len);
    // Allocate buffer
    std::vector<char> output(utf8_len);
    // Convert
    simdutf::result res = simdutf::convert_utf32_to_utf8(input.data(), len, output.data());
    // res.error is zero on success; we assume valid input
    // Compute checksum over the output bytes
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(output.data());
    const unsigned char* end = ptr + utf8_len;
    for (; ptr != end; ++ptr) {
        hash ^= static_cast<uint64_t>(*ptr);
        hash *= 1099511628211ULL;
    }
    return hash;
}