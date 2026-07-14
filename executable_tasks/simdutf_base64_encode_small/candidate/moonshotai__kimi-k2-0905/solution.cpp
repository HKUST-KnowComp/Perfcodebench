#include "interface.h"
#include "simdutf.h"
#include <string>
#include <cstdint>

namespace {
inline uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
    const size_t max_out_len = simdutf::base64_length_from_binary(input.size(), simdutf::base64_default);
    std::string out;
    out.resize(max_out_len);
    size_t written = simdutf::binary_to_base64(input.data(), input.size(), out.data(), simdutf::base64_default);
    out.resize(written);
    return checksum_bytes(out.data(), out.size());
}