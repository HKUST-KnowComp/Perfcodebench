#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

// FNV-1a 64-bit hash implementation
inline uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    const uint8_t* end = ptr + len;
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate the required capacity for the UTF-8 output.
    // UTF-16 to UTF-8 can result in up to 3 bytes per code unit (for BMP) 
    // or 4 bytes per surrogate pair (2 code units). 
    // simdutf::utf8_length_from_utf16 provides the exact length.
    size_t utf8_len = simdutf::utf8_length_from_utf16(reinterpret_cast<const char16_t*>(input.data()), input.size());
    
    // Use a stack-allocated buffer for small strings or a vector for large ones.
    // Given the 'xlarge' task ID, we use a vector to ensure safety.
    std::vector<char> buffer(utf8_len);

    // Perform the SIMD-accelerated transcoding.
    size_t written = simdutf::convert_utf16_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        buffer.data()
    );

    // Calculate the checksum over the resulting UTF-8 bytes.
    return checksum_bytes(buffer.data(), written);
}