#include "interface.h"
#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    std::vector<char> out;
    out.resize(simdutf::maximal_binary_length_from_base64(input.data(), input.size()));
    size_t written = simdutf::base64_to_binary(input.data(), input.size(), out.data(), simdutf::base64_default);
    return checksum_bytes(out.data(), written);
}