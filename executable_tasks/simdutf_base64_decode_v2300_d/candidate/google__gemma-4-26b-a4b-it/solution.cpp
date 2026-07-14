#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as per the baseline requirement
uint64_t checksum_bytes(const std::string& data) {
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
        return checksum_bytes("");
    }

    // simdutf::base64::unpack is highly optimized for various architectures.
    // It handles padding and returns a std::string containing the decoded bytes.
    // We use the version that returns a string to match the baseline's behavior.
    // Note: simdutf::base64::unpack expects a valid base64 string.
    
    std::string decoded;
    try {
        // simdutf::base64::unpack returns a std::string containing the decoded data.
        // It is significantly faster than manual scalar decoding.
        decoded = simdutf::base64::unpack(input);
    } catch (...) {
        // In case of malformed input, though the task implies valid base64.
        return 0;
    }

    return checksum_bytes(decoded);
}