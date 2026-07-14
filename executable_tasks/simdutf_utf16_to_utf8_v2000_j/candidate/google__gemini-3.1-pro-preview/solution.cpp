#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>
#include <cstdint>

namespace {

/**
 * FNV-1a 64-bit hash implementation for checksumming the UTF-8 byte sequence.
 */
inline uint64_t compute_fnv1a(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // 1. Calculate the exact length required for the UTF-8 output.
    // simdutf provides an optimized way to count the bytes needed.
    size_t utf8_len = simdutf::utf8_length_from_utf16le(reinterpret_cast<const char16_t*>(input.data()), input.size());

    // 2. Allocate a buffer for the UTF-8 output.
    // Using std::vector<uint8_t> for automatic memory management and alignment.
    std::vector<uint8_t> buffer(utf8_len);

    // 3. Perform the SIMD-accelerated transcoding.
    // convert_utf16le_to_utf8 returns the number of bytes written.
    size_t written = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        reinterpret_cast<char*>(buffer.data())
    );

    // 4. Compute the checksum on the resulting UTF-8 bytes.
    // The task requires the checksum over the exact UTF-8 sequence.
    return compute_fnv1a(buffer.data(), written);
}