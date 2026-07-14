#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>
#include <string>

/**
 * Optimized UTF-16LE to UTF-8 transcoding and checksum calculation.
 * Uses the simdutf library for SIMD-accelerated transcoding.
 */

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // 1. Determine the exact length of the UTF-8 output.
    // simdutf uses SIMD to scan the UTF-16 string and calculate the output size.
    size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.length());

    // 2. Allocate a buffer for the UTF-8 output.
    // Using a raw buffer or std::vector for performance.
    std::vector<char> buffer(utf8_len);

    // 3. Perform the actual transcoding.
    // simdutf::convert_utf16le_to_utf8 is highly optimized for various architectures.
    size_t actual_utf8_len = simdutf::convert_utf16le_to_utf8(
        input.data(), 
        input.length(), 
        buffer.data()
    );

    // 4. Calculate the FNV-1a checksum over the UTF-8 bytes.
    // FNV-1a constants for 64-bit.
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buffer.data());
    const uint8_t* end = ptr + actual_utf8_len;

    // Tight loop for checksum calculation.
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= fnv_prime;
    }

    return hash;
}
