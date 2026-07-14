#include "interface.h"

#include <simdutf.h>
#include <cstddef>
#include <cstdint>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    // Precompute maximum possible output length for the given input
    size_t max_len = simdutf::base64_maximum_output_length(input.size());
    
    // Allocate a buffer for the decoded binary data
    std::string out(max_len, '\0');
    
    // Perform SIMD-accelerated base64 decoding
    auto result = simdutf::base64_decode(input.data(), input.size(), out.data());
    size_t decoded_len = result.count;  // actual number of bytes decoded
    
    // Compute checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(out.data());
    for (size_t i = 0; i < decoded_len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}