#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a hash implementation as per the baseline requirement
inline uint64_t checksum_bytes(const std::string& data) {
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

    // simdutf::base64::unpack_padded expects a null-terminated string or a specific length.
    // Since we are given a std::string, we use the version that takes a length.
    // We need to handle the potential error/padding correctly.
    
    size_t max_out_size = (input.size() / 4) * 3;
    std::string decoded_buffer;
    decoded_buffer.resize(max_out_size);
    
    size_t decoded_size = 0;
    try {
        // simdutf::base64::unpack_padded is highly optimized for standard base64 with padding
        decoded_size = simdutf::base64::unpack_padded(input.data(), input.size(), decoded_buffer.data());
    } catch (...) {
        // Fallback or error handling if input is malformed, though task implies valid base64
        return 0;
    }

    // Resize to actual decoded length to ensure checksum matches exact bytes
    decoded_buffer.resize(decoded_size);
    
    return checksum_bytes(decoded_buffer);
}