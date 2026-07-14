#include "interface.h"
#include <string>
#include <simdutf.h>

namespace {
uint64_t fnv1a(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
    size_t input_len = input.size();
    size_t output_len = ((input_len + 2) / 3) * 4;
    std::string out(output_len, '\0');
    size_t written = simdutf::base64::encode(input.data(), input_len, &out[0]);
    return fnv1a(out.data(), written);
}
