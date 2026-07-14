#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>

uint64_t encode_checksum(const std::string& input) {
    size_t input_len = input.size();
    size_t output_len = ((input_len + 2) / 3) * 4;
    std::vector<char> output(output_len);
    size_t actual_out_len = simdutf::base64::encode(input.data(), input_len, output.data());
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < actual_out_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(output[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}