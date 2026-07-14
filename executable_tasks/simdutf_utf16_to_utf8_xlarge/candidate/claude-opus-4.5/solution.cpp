#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstddef>
#include <memory>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Use simdutf to compute required UTF-8 length
    size_t utf8_len = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char16_t*>(input.data()), input.size());

    // Allocate buffer for UTF-8 output
    std::unique_ptr<char[]> utf8_buf(new char[utf8_len]);

    // Perform SIMD-accelerated transcoding
    size_t written = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), input.size(),
        utf8_buf.get());

    // FNV-1a checksum with unrolling for ILP
    const unsigned char* data = reinterpret_cast<const unsigned char*>(utf8_buf.get());
    size_t len = written;
    
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    // Process 8 bytes at a time for better ILP
    size_t i = 0;
    size_t limit = len & ~size_t(7);
    for (; i < limit; i += 8) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 1]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 2]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 3]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 4]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 5]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 6]);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(data[i + 7]);
        hash *= FNV_PRIME;
    }

    // Handle remaining bytes
    for (; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}