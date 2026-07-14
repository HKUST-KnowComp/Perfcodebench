#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a hash implementation to match the baseline checksum logic
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
    std::vector<uint8_t> decoded_buffer;
    try {
        // simdutf::base64::unpack_padded returns a std::vector<uint8_t>
        // or we can use the version that writes to a pre-allocated buffer.
        // For simplicity and safety with the provided API, we use the vector return.
        decoded_buffer = simdutf::base64::unpack_padded(input.data(), input.size());
    } catch (...) {
        // Fallback or error handling if input is malformed, 
        // though task implies valid base64.
        return 0;
    }

    // Convert vector to string view/string for the checksum function
    // Since checksum_bytes takes const std::string&, we construct a string.
    // To avoid extra copies, we can refactor checksum_bytes to take a span/pointer,
    // but to strictly follow the 'preserve correctness' and 'interface' rules,
    // we ensure the checksum is calculated over the exact bytes.
    
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t ch : decoded_buffer) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}