#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstddef>

namespace {

uint64_t fnv1a(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // anonymous namespace

uint64_t decode_checksum(const std::string& input) {
    size_t input_len = input.size();
    size_t max_decoded_len = simdutf::base64::decoded_length(input_len);
    std::vector<char> buffer(max_decoded_len);
    size_t decoded_len = simdutf::base64::decode(input.data(), input_len, buffer.data());
    return fnv1a(buffer.data(), decoded_len);
}
