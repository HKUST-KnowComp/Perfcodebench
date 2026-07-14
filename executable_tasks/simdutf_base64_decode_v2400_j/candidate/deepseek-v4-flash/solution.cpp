#include "interface.h"
#include <string>
#include <cstddef>
#include <vector>
#include <simdutf.h>

namespace {
uint64_t fnv1a_64(const uint8_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}
}  // namespace

uint64_t decode_checksum(const std::string& input) {
    size_t max_decoded = ((input.size() + 3) / 4) * 3;
    std::vector<char> output(max_decoded);
    size_t decoded_length = simdutf::base64::decode(input.data(), input.size(), output.data());
    return fnv1a_64(reinterpret_cast<const uint8_t*>(output.data()), decoded_length);
}
