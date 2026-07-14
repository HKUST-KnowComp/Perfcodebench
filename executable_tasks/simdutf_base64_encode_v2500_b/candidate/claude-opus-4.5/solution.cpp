#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t encode_checksum(const std::string& input) {
    // Calculate output size for base64 encoding
    size_t output_len = simdutf::base64_length_from_binary(input.size());
    
    // Allocate output buffer
    std::string out(output_len, '\0');
    
    // Use simdutf's SIMD-accelerated base64 encoding
    size_t written = simdutf::binary_to_base64(
        input.data(),
        input.size(),
        &out[0]
    );
    
    // Resize to actual written size (should match output_len)
    out.resize(written);
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}