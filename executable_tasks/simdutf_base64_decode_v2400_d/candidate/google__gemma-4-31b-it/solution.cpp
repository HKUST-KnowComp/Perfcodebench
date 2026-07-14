#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

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

    // Calculate the maximum possible decoded size
    size_t max_decoded_size = (input.size() * 3) / 4;
    std::vector<uint8_t> buffer(max_decoded_size);

    // Use simdutf for high-performance base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data(), buffer.size());

    if (result.error) {
        // In case of decoding error, the behavior is undefined by the task, 
        // but we return the initial hash or handle it gracefully.
        return 0;
    }

    return compute_checksum(buffer.data(), result.decoded_size);
}