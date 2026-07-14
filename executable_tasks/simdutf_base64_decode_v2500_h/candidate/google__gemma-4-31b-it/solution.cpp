#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Calculate the maximum possible decoded size
    size_t max_decoded_size = (input.size() * 3) / 4;
    
    // Use a stack buffer for small inputs to avoid heap allocation
    uint8_t stack_buf[1024];
    uint8_t* buffer = stack_buf;
    std::vector<uint8_t> heap_buf;
    
    if (max_decoded_size > 1024) {
        heap_buf.resize(max_decoded_size);
        buffer = heap_buf.data();
    }

    // Use simdutf for high-performance base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer);
    
    if (result.error) {
        // In case of decoding error, the contract implies we should handle it.
        // Based on the baseline, we assume valid base64 or return 0/current hash.
        return 0;
    }

    // Calculate FNV-1a style checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer;
    size_t len = result.length;
    
    // Unroll loop for performance
    while (len >= 8) {
        hash ^= static_cast<uint64_t>(data[0]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[1]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[2]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[3]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[4]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[5]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[6]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[7]); hash *= 1099511628211ULL;
        data += 8;
        len -= 8;
    }
    while (len--) {
        hash ^= static_cast<uint64_t>(*data++);
        hash *= 1099511628211ULL;
    }

    return hash;
}