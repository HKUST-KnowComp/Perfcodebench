#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

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
    const size_t max_binary_len = (input.size() / 4) * 3;
    std::vector<char> buffer(max_binary_len);

    simdutf::result res = simdutf::base64_to_binary(
        input.data(),
        input.size(),
        buffer.data(),
        simdutf::base64_default
    );

    if (res.error != simdutf::error_code::SUCCESS) {
        // Invalid base64; fall back to empty checksum
        return checksum_bytes(nullptr, 0);
    }

    return checksum_bytes(buffer.data(), res.count);
}