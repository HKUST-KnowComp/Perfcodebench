#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-calculate maximum possible output size
    std::vector<uint8_t> decoded_buffer(input.size() * 3 / 4 + 3);
    
    // Use simdutf for high-performance SIMD-accelerated decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), decoded_buffer.data(), decoded_buffer.size());
    
    if (result.error) {
        // In a real scenario, handle error. For this task, we assume valid input per baseline.
        return 0;
    }

    // FNV-1a 64-bit checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = decoded_buffer.data();
    size_t length = result.bytes_decoded;
    
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}