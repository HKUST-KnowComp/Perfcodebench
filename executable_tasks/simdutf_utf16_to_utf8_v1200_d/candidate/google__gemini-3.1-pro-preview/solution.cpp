#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>
#include <cstdint>

/**
 * Performance-optimized UTF-16LE to UTF-8 transcoding and checksum calculation.
 * Uses simdutf for SIMD-accelerated transcoding and an unrolled FNV-1a hash.
 */

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // 1. Determine the required size for the UTF-8 output.
    // simdutf::utf8_length_from_utf16 is highly optimized.
    size_t utf8_len = simdutf::utf8_length_from_utf16(input.data(), input.size());

    // 2. Allocate buffer for UTF-8 output.
    // Using std::vector<char> for automatic memory management and alignment.
    std::vector<char> buffer(utf8_len);

    // 3. Perform the transcoding.
    // convert_utf16_to_utf8 returns the number of bytes written.
    size_t written = simdutf::convert_utf16_to_utf8(input.data(), input.size(), buffer.data());

    // 4. Calculate FNV-1a checksum over the UTF-8 bytes.
    // FNV-1a constants
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = FNV_OFFSET_BASIS;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    size_t i = 0;

    // Unroll the loop to improve instruction-level parallelism and reduce branch overhead.
    for (; i + 7 < written; i += 8) {
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

    // Process remaining bytes.
    for (; i < written; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}