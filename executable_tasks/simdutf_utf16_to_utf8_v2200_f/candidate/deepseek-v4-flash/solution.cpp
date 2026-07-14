#include "interface.h"
#include <string>
#include <cstdint>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
    size_t input_len = input.size();
    // Each UTF-16 code unit can expand to at most 3 UTF-8 bytes.
    size_t max_utf8_len = input_len * 3;
    std::string utf8_out(max_utf8_len, '\0');
    auto result = simdutf::convert_utf16le_to_utf8(input.data(), input_len, utf8_out.data());
    // Input is guaranteed valid, but we guard against unexpected errors.
    if (result.error) {
        return 0;
    }
    uint64_t hash = 1469598103934665603ULL;
    const char* data = utf8_out.data();
    size_t out_len = result.count;
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
