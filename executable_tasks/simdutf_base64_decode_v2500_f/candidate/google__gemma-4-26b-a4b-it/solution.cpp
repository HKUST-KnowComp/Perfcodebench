#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation to match the original checksum logic
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
    // It is significantly faster than scalar implementations due to SIMD usage.
    // We use a vector to manage the decoded buffer memory efficiently.
    std::vector<uint8_t> decoded_buffer;
    
    // Pre-calculating approximate size to avoid multiple reallocations
    decoded_buffer.resize((input.size() * 3) / 4);

    size_t decoded_size = simdutf::base64::unpack_padded(input.data(), input.size(), decoded_buffer.data());
    
    // Resize to the actual number of bytes decoded (removing padding effects)
    decoded_buffer.resize(decoded_size);

    // Convert vector back to string view/string for the checksum function
    // Since checksum_bytes takes std::string, we construct a string from the buffer.
    // Note: In a real production environment, we would refactor checksum_bytes 
    // to take std::string_view to avoid this copy, but we must preserve correctness.
    std::string out(reinterpret_cast<const char*>(decoded_buffer.data()), decoded_size);

    return checksum_bytes(out);
}