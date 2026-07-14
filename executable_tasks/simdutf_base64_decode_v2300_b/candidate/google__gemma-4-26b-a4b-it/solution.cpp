#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a 64-bit hash implementation
inline uint64_t compute_fnv1a(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return compute_fnv1a("");
    }

    // simdutf::base64::unpack returns the decoded bytes in a std::string
    // It is highly optimized using SIMD instructions.
    try {
        std::string decoded = simdutf::base64::unpack(input);
        return compute_fnv1a(decoded);
    } catch (...) {
        // Fallback or error handling if input is malformed, 
        // though task implies valid base64.
        return 0;
    }
}
