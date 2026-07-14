#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

// FNV-1a constants for 64-bit hashing
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t compute_fnv1a(const uint8_t* data, size_t len) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return FNV_OFFSET_BASIS;
    }

    // 1. Calculate the required buffer size for UTF-8 output.
    // simdutf provides an efficient way to calculate this.
    size_t utf8_len = simdutf::utf8_length_from_utf16(reinterpret_cast<const char16_t*>(input.data()), input.length());

    // 2. Allocate a buffer for the UTF-8 result.
    // Using a vector of uint8_t for raw byte access.
    std::vector<uint8_t> buffer(utf8_len);

    // 3. Perform the SIMD-accelerated transcoding.
    // convert_utf16_to_utf8 returns the number of bytes written.
    size_t written = simdutf::convert_utf16_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.length(), 
        reinterpret_cast<char*>(buffer.data())
    );

    // 4. Compute the checksum over the resulting bytes.
    return compute_fnv1a(buffer.data(), written);
}