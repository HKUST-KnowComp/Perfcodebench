#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t encode_checksum(const std::string& input) {
    // Calculate output size for base64 encoding
    size_t output_len = simdutf::base64_length_from_binary(input.size());
    
    // Allocate output buffer
    std::string out(output_len, '\0');
    
    // Encode using simdutf (SIMD-accelerated)
    simdutf::binary_to_base64(input.data(), input.size(), out.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}