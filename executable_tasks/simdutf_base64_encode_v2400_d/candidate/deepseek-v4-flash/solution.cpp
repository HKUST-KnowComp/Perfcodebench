#include "interface.h"
#include <simdutf.h>
#include <string>

uint64_t encode_checksum(const std::string& input) {
    // Use SIMD-accelerated base64 encoding from simdutf library
    std::string base64 = simdutf::base64::encode(input);
    
    // Compute FNV-1a 64-bit checksum over the encoded output
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : base64) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}
