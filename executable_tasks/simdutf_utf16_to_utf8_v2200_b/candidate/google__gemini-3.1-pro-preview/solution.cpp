#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>
#include <cstdint>

/**
 * Performance-optimized UTF-16LE to UTF-8 transcoding using simdutf.
 * The task requires transcoding a valid UTF-16LE string and returning an FNV-1a checksum.
 */

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL; // FNV offset basis
    }

    // 1. Determine the required size for the UTF-8 output.
    // simdutf provides a fast way to calculate the exact length.
    size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());

    // 2. Allocate a buffer for the UTF-8 output.
    // Using std::vector<char> for a contiguous buffer.
    std::vector<char> buffer(utf8_len);

    // 3. Perform the SIMD-accelerated transcoding.
    // The input is guaranteed to be valid UTF-16LE per task context.
    size_t written = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.data());

    // 4. Calculate the FNV-1a 64-bit checksum over the UTF-8 bytes.
    // FNV-1a constants:
    // Offset basis: 1469598103934665603
    // Prime: 1099511628211
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buffer.data());
    const uint8_t* end = ptr + written;

    // Unrolling or SIMD hashing could be applied here, but standard FNV-1a 
    // is usually bottlenecked by the transcoding step for medium inputs.
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= fnv_prime;
    }

    return hash;
}