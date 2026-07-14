#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-allocate buffer for decoded data to avoid multiple allocations
    // Base64 decoded size is roughly 3/4 of input size
    std::vector<uint8_t> buffer;
    buffer.resize((input.size() * 3) / 4 + 3);

    // Use simdutf for high-performance SIMD-accelerated base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data(), buffer.size());

    if (result.error) {
        // In case of error, the contract implies we should handle it gracefully
        // though standard base64 inputs are expected.
        return 0;
    }

    // Calculate FNV-1a checksum directly on the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    size_t length = result.bytes_decoded;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}