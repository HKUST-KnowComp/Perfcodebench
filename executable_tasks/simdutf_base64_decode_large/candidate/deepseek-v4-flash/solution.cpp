#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input) {
    size_t decoded_length = simdutf::base64::decode_length(input);
    std::vector<char> buffer(decoded_length);
    simdutf::result r = simdutf::base64::decode(input, buffer.data(), simdutf::base64_default);
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < r.count; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}