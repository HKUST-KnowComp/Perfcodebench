#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a hash implementation as per the original requirement
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

    // simdutf::base64::unpack_padded handles standard base64 with padding.
    // We use a vector to manage the decoded buffer memory efficiently.
    // The maximum possible size for decoded base64 is 3/4 of the input size.
    std::string decoded_output;
    try {
        // simdutf's decode function is highly optimized using SIMD (AVX2/SSE/NEON)
        // We use the version that returns a string or writes to a buffer.
        // For simplicity and correctness with the existing interface, 
        // we use the string-based API if available or decode into a buffer.
        
        // Note: simdutf::base64::unpack_padded is the standard way to handle this.
        // We need to ensure we handle the output correctly for the checksum.
        
        // Pre-calculating size to avoid reallocations
        size_t max_out_size = (input.size() * 3) / 4 + 1;
        decoded_output.resize(max_out_size);
        
        size_t decoded_size = simdutf::base64::unpack_padded(input.data(), input.size(), 
                                                            reinterpret_cast<uint8_t*>(&decoded_output[0]));
        
        decoded_output.resize(decoded_size);
    } catch (...) {
        // Fallback or error handling if input is malformed, though task implies valid base64
        return 0;
    }

    return checksum_bytes(decoded_output);
}
