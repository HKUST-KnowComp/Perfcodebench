#include "interface.h"
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input) {
    size_t max_output = (input.size() / 4) * 3 + 3;
    std::string output(max_output, '\0');
    size_t decoded_len = simdutf::base64_to_binary(input.data(), input.size(), output.data(), simdutf::base64_default);
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < decoded_len; ++i) {
        unsigned char ch = static_cast<unsigned char>(output[i]);
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
