#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

/**
 * Computes the FNV-1a 64-bit hash over a sequence of char16_t.
 * This matches the logic in the baseline solution.
 */
inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_utf16(nullptr, 0);
    }

    // 1. Determine the number of UTF-16 code units needed.
    // simdutf provides an optimized way to calculate the output length.
    size_t utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());

    // 2. Allocate a buffer for the UTF-16 output.
    // Using a vector for automatic memory management.
    std::vector<char16_t> output_buffer(utf16_length);

    // 3. Perform the conversion using SIMD-accelerated simdutf.
    // The task guarantees valid UTF-8, so we use the convert function.
    // Note: convert_utf8_to_utf16 returns the number of char16_t written.
    size_t written = simdutf::convert_utf8_to_utf16(input.data(), input.size(), output_buffer.data());

    // 4. Compute and return the checksum over the converted data.
    return checksum_utf16(output_buffer.data(), written);
}