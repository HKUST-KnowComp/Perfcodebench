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

    // Pre-calculate maximum possible size to avoid multiple allocations
    std::vector<uint8_t> buffer(input.size());
    
    // Use simdutf for high-performance SIMD base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data());
    
    if (result.error) {
        // In case of decoding error, the behavior should match the baseline's 
        // implicit handling or return a default. Given the task, we assume valid input.
        return 0;
    }

    return checksum_bytes(buffer.data(), result.length);
}