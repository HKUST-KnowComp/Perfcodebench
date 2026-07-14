#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstddef>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    // Maximum possible decoded length (actual output may be shorter due to padding)
    const size_t max_decoded = simdutf::base64_max_decoded_length(input.size());
    std::vector<char> buffer(max_decoded);
    // Decode using SIMD-accelerated base64 decoder
    const size_t decoded = simdutf::base64_decode(input.data(), input.size(), buffer.data());
    // Compute checksum over the exact decoded output
    return checksum_bytes(reinterpret_cast<const uint8_t*>(buffer.data()), decoded);
}
