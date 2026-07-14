#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-calculate maximum possible size for the decoded output
    std::vector<uint8_t> decoded_buffer(input.size());
    
    // Use simdutf for high-performance SIMD-accelerated base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), decoded_buffer.data());
    
    if (result.error) {
        // In a real scenario, handle error. For this task, we assume valid input per contract.
        return 0;
    }

    // FNV-1a 64-bit hash over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = decoded_buffer.data();
    size_t length = result.length;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}