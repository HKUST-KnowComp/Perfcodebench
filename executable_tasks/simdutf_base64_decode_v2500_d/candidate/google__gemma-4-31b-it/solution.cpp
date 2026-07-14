#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-calculate the maximum possible size for the decoded output
    size_t max_decoded_size = (input.size() * 3) / 4;
    std::vector<uint8_t> buffer(max_decoded_size);

    // Use simdutf for high-performance SIMD-accelerated base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data());

    if (result.error) {
        // In case of decoding error, the contract implies we should handle it.
        // Based on the baseline, we assume valid base64 or handle gracefully.
        return 0;
    }

    // Calculate FNV-1a style checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    size_t length = result.bytes_decoded;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}