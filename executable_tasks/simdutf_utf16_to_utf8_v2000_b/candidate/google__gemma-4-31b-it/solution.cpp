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
    if (input.empty()) return 1469598103934665603ULL; // Initial hash for empty string if required, but usually checksums loop over data
    
    // Use simdutf to determine the required size for the UTF-8 output
    // The input is UTF-16LE (std::u16string is typically native endian)
    size_t utf8_len = simdutf::utf16_to_utf8_size(reinterpret_cast<const char16_t*>(input.data()), input.size());
    
    // Allocate a buffer for the result
    std::vector<uint8_t> buffer(utf8_len);
    
    // Perform the actual transcoding
    simdutf::utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data(), utf8_len);
    
    // Calculate checksum over the resulting bytes
    // Note: The baseline checksum_bytes starts with 1469598103934665603ULL and iterates over the string
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t ch : buffer) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}