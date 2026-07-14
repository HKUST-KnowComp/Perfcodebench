#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-calculate max possible size to avoid reallocations
    size_t max_decoded_size = (input.size() / 4) * 3;
    std::vector<uint8_t> buffer(max_decoded_size);

    // Use simdutf for high-performance SIMD base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data());

    if (result.error) {
        // In case of decoding error, the contract implies we should handle it.
        // Given the task, we assume valid base64 or return current hash state.
        return 1469598103934665603ULL;
    }

    // Calculate FNV-1a checksum directly on the decoded buffer
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    size_t length = result.bytes_decoded;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}