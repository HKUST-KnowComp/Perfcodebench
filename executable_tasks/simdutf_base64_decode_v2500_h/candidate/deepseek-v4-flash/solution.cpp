#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t decode_checksum(const std::string& input) {
    size_t max_len = simdutf::base64::max_bin_length(input.size());
    std::vector<char> buffer(max_len);
    size_t decoded_len = simdutf::base64::decode(input.data(), input.size(), buffer.data());
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < decoded_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
