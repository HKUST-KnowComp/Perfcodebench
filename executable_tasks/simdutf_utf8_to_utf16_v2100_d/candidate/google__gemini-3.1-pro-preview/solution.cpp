#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

namespace {

/**
 * FNV-1a 64-bit hash implementation for UTF-16 code units.
 */
inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    const char16_t* end = data + length;
    for (; data < end; ++data) {
        hash ^= static_cast<uint64_t>(*data);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate the required length for the UTF-16 output.
    // simdutf::utf16_length_from_utf8 is very fast.
    size_t utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());

    // Use a stack buffer for small inputs or a heap-allocated buffer for larger ones.
    // Given the 'xlarge' task description, we use a vector for safety and simplicity,
    // but we ensure we only allocate once.
    std::vector<char16_t> buffer(utf16_length);

    // Perform the SIMD-accelerated transcoding.
    // This function handles the conversion to UTF-16LE (native on most platforms).
    size_t written = simdutf::convert_utf8_to_utf16(input.data(), input.size(), buffer.data());

    // Calculate the checksum over the resulting UTF-16 sequence.
    return checksum_utf16(buffer.data(), written);
}
