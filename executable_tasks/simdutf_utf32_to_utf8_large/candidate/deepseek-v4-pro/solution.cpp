#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    std::vector<char> output(input.size() * 4);
    size_t out_len = simdutf::convert_valid_utf32_to_utf8(input.data(), input.size(), output.data());
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(output[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}