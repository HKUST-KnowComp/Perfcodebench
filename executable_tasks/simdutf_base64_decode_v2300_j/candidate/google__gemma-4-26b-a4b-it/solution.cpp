#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a 64-bit hash implementation
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

    // simdutf::base64::unpack_padded handles standard base64 with padding.
    // We use a vector to manage the decoded buffer memory efficiently.
    // The size of the decoded buffer is at most 3/4 of the input size.
    std::string decoded_output;
    try {
        // simdutf::base64::unpack_padded returns a std::string or similar container
        // depending on the version, but the standard way to get the bytes is via the API.
        // We use the API that decodes into a pre-allocated buffer or returns a string.
        decoded_output = simdutf::base64::unpack_padded(input);
    } catch (...) {
        // Fallback or error handling if input is malformed, though task implies valid base64
        return 0;
    }

    return checksum_bytes(decoded_output);
}
