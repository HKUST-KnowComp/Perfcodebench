#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {
uint64_t fnv1a(const char* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
}

uint64_t encode_checksum(const std::string& input) {
    size_t input_size = input.size();
    size_t output_size = ((input_size + 2) / 3) * 4;
    std::vector<char> output(output_size);
    size_t written = simdutf::base64::encode(input.data(), input_size, output.data());
    return fnv1a(output.data(), written);
}