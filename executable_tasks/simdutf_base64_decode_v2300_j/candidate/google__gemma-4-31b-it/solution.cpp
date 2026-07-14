#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-calculate maximum possible output size
    size_t max_out_len = (input.size() / 4) * 3;
    std::vector<uint8_t> buffer(max_out_len);

    // Use simdutf for high-performance SIMD base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data(), buffer.size());

    if (result.error != simdutf::base64::decode_result::error::none) {
        // In a real scenario, handle error. For this task, we assume valid input per baseline.
        return 0;
    }

    // Calculate FNV-1a style checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    size_t len = result.length;

    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}