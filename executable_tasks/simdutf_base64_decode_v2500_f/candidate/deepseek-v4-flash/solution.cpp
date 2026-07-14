#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t decode_checksum(const std::string& input) {
    size_t max_out = (input.size() / 4) * 3 + 3;
    std::string output;
    output.resize(max_out);
    size_t decoded_len = simdutf::base64::decode(input.data(), input.size(), &output[0]);
    output.resize(decoded_len);
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < decoded_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(output[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
