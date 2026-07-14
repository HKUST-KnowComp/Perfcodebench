#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-allocate buffer for decoded data
    std::vector<uint8_t> buffer(input.size());
    
    // Use simdutf for high-performance base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data());
    
    if (result.error != simdutf::base64::decode_result::error::none) {
        // In a real scenario, handle error. For this task, we assume valid input per baseline.
        return 0;
    }

    return checksum_bytes(buffer.data(), result.length);
}