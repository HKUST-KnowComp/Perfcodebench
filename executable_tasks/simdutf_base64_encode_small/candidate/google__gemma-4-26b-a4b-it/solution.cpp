#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as per the baseline
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
    size_t output_len = simdutf::base64::encoded_size(input.size());
    std::string out(output_len, '\0');

    // Use simdutf's highly optimized encoding
    // Note: simdutf::base64::encode returns the number of bytes written
    size_t written = simdutf::base64::encode(out.data(), input.data(), input.size());
    
    // Resize string to actual written length (handles potential padding differences if any,
    // though encoded_size is exact for standard base64)
    out.resize(written);

    return checksum_bytes(out);
}
