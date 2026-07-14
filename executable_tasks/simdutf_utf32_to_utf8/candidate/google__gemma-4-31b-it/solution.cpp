#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate the maximum possible size for UTF-8 output (4 bytes per UTF-32 code point)
    size_t max_size = input.size() * 4;
    std::vector<uint8_t> buffer(max_size);

    // Use simdutf for high-performance vectorized transcoding
    // convert_utf32_to_utf8 returns the number of bytes written to the destination
    auto result = simdutf::convert_utf32_to_utf8(
        reinterpret_cast<const uint32_t*>(input.data()),
        input.size(),
        buffer.data(),
        max_size
    );

    if (result.error) {
        // The task guarantees valid UTF-32, but we handle error for completeness
        return 0;
    }

    // Compute the FNV-1a style checksum over the resulting UTF-8 byte sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    size_t length = result.bytes_written;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}