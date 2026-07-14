#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a 64-bit hash implementation
uint64_t checksum_bytes(const std::string& data) {
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
    size_t written = simdutf::base64::encode(reinterpret_cast<const uint8_t*>(input.data()), input_len, reinterpret_cast<uint8_t*>(&out[0]));

    // If for some reason the output is shorter than the reserved size (though standard base64 is fixed length),
    // we ensure the string reflects the actual written bytes for the checksum.
    if (written < output_len) {
        out.resize(written);
    }

    return checksum_bytes(out);
}
