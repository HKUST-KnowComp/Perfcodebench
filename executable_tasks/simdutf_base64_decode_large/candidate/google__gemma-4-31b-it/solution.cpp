#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Use a vector to hold the decoded bytes to avoid std::string overhead
    // simdutf::base64_decode returns the number of bytes decoded
    std::vector<uint8_t> decoded_buffer(input.size());
    
    auto result = simdutf::base64_decode(input.data(), input.size(), decoded_buffer.data());
    
    if (result.error != simdutf::base64_decode_result::success) {
        // In a real scenario, handle error. For this task, we assume valid input per baseline.
        return 0;
    }

    // Compute FNV-1a checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = decoded_buffer.data();
    size_t length = result.bytes_decoded;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}