#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation
uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.data());
    size_t len = data.size();
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

    // simdutf::base64::encode uses SIMD instructions to perform high-speed encoding.
    // It returns a std::string containing the base64 encoded data.
    std::string encoded = simdutf::base64::encode(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    
    return checksum_bytes(encoded);
}
