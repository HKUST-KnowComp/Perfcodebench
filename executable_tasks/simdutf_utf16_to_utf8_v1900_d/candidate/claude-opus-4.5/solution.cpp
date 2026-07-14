#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstddef>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate required UTF-8 buffer size
    size_t utf8_len = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char16_t*>(input.data()), input.size());

    // Allocate buffer and perform SIMD-accelerated transcoding
    std::string out(utf8_len, '\0');
    simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), input.size(),
        out.data());

    // FNV-1a checksum with 8x unrolling
    const uint64_t FNV_PRIME = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
    size_t len = out.size();
    size_t i = 0;

    // Process 8 bytes at a time
    for (; i + 8 <= len; i += 8) {
        hash ^= data[i];     hash *= FNV_PRIME;
        hash ^= data[i + 1]; hash *= FNV_PRIME;
        hash ^= data[i + 2]; hash *= FNV_PRIME;
        hash ^= data[i + 3]; hash *= FNV_PRIME;
        hash ^= data[i + 4]; hash *= FNV_PRIME;
        hash ^= data[i + 5]; hash *= FNV_PRIME;
        hash ^= data[i + 6]; hash *= FNV_PRIME;
        hash ^= data[i + 7]; hash *= FNV_PRIME;
    }

    // Handle remaining bytes
    for (; i < len; ++i) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }

    return hash;
}