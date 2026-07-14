#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) return 1469598103934665603ULL; // Initial hash for empty string

    // Pre-calculate required size to avoid multiple allocations
    // utf16_to_utf8 can be used to find the required size
    size_t utf8_len = simdutf::utf16_to_utf8_size(reinterpret_cast<const char16_t*>(input.data()), input.size());
    
    std::vector<uint8_t> buffer(utf8_len);
    simdutf::utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data(), utf8_len);

    // The baseline checksum logic is applied to the resulting bytes
    // Note: The baseline checksum_bytes function starts with the seed and iterates
    // We must ensure the seed is handled correctly. The baseline logic is:
    // uint64_t hash = 1469598103934665603ULL; for (unsigned char ch : data) { ... }
    
    // Since the baseline checksum_bytes is a separate function, we replicate its logic here
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf8_len; ++i) {
        hash ^= static_cast<uint64_t>(buffer[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}