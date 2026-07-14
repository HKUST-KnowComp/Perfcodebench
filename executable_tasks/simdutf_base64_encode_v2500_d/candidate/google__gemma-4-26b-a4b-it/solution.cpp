#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation for the checksum
inline uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_bytes("");
    }

    // Calculate required size for base64 output
    size_t input_len = input.size();
    size_t output_len = ((input_len + 2) / 3) * 4;
    
    std::string out;
    out.resize(output_len);

    // Use simdutf for high-performance SIMD-accelerated base64 encoding
    // simdutf::base64::encode returns the number of bytes written
    size_t encoded_size = simdutf::base64::encode(out.data(), input.data(), input_len);
    
    // If the encoded size is less than the reserved size (due to padding logic in simdutf),
    // we ensure the string reflects the actual content. 
    // However, simdutf::base64::encode for standard base64 usually fills the full buffer.
    // To be safe and match the exact output expected by the checksum:
    if (encoded_size < out.size()) {
        out.resize(encoded_size);
    }

    return checksum_bytes(out);
}
