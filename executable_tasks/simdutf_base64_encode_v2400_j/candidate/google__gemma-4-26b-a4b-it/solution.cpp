#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// Optimized FNV-1a checksum implementation
uint64_t checksum_bytes(const std::string& data) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data.data());
    size_t len = data.size();
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_bytes("");
    }

    // Use simdutf for high-performance SIMD-accelerated base64 encoding
    // simdutf::base64::encode returns a std::string
    std::string encoded = simdutf::base64::encode(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    
    return checksum_bytes(encoded);
}
