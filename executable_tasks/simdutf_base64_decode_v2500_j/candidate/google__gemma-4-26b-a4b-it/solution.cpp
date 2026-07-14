#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation for correctness
uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_bytes("");
    }

    // Calculate maximum possible output size to avoid reallocations
    size_t max_out_size = (input.size() * 3) / 4;
    std::string decoded_buffer;
    decoded_buffer.resize(max_out_size);

    size_t decoded_len = 0;
    try {
        // Use simdutf for high-performance SIMD-accelerated base64 decoding
        // simdutf::base64::unpack returns the number of bytes written
        decoded_len = simdutf::base64::unpack(input.data(), input.size(), decoded_buffer.data());
    } catch (...) {
        // Fallback or error handling if input is malformed, though task implies valid base64
        return 0;
    }

    // Resize the buffer to the actual decoded length to ensure checksum is correct
    decoded_buffer.resize(decoded_len);

    return checksum_bytes(decoded_buffer);
}
