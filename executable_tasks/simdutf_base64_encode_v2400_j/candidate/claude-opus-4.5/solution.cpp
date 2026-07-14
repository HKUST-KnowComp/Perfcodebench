#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t encode_checksum(const std::string& input) {
    // Calculate required output size for base64 encoding
    size_t output_len = simdutf::base64_length_from_binary(input.size());
    
    // Allocate output buffer
    std::string out(output_len, '\0');
    
    // Perform SIMD-accelerated base64 encoding
    simdutf::binary_to_base64(input.data(), input.size(), out.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(out.data());
    const unsigned char* end = ptr + output_len;
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= 1099511628211ULL;
    }
    return hash;
}