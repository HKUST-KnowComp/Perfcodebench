#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

// FNV-1a 64-bit hash implementation
uint64_t compute_checksum(const uint8_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Pre-allocate buffer for decoded data to avoid multiple reallocations
    // Base64 decoding produces roughly 3/4 the size of the input
    std::vector<uint8_t> decoded_buffer;
    decoded_buffer.resize((input.size() / 4) * 3 + 3);

    // Use simdutf's base64 decode implementation for high performance
    // simdutf::base64::decode returns the number of bytes written to the output buffer
    auto result = simdutf::base64::decode(input.data(), input.size(), decoded_buffer.data());

    if (result.error) {
        // In case of decoding error, the behavior should be consistent with the baseline
        // though the baseline didn't explicitly handle invalid chars beyond returning -1.
        // For the purpose of this task, we assume valid base64 input as per the goal.
        return 0;
    }

    return compute_checksum(decoded_buffer.data(), result.bytes_written);
}
