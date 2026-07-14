#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

/**
 * Performance Engineer Optimization:
 * 1. Use simdutf::convert_utf16_to_utf8 for high-performance SIMD-accelerated transcoding.
 * 2. Pre-calculate or over-allocate the output buffer to avoid std::string's growth overhead.
 * 3. Use a tight loop for the FNV-1a checksum calculation on the raw output buffer.
 */

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // UTF-16 to UTF-8 expansion is at most 3 bytes per 2-byte code unit 
    // (except for surrogate pairs which are 4 bytes for 4 bytes of input).
    // A safe upper bound is input.size() * 3.
    size_t max_utf8_len = input.size() * 3;
    
    // Use a unique_ptr for a raw buffer to avoid zero-initialization overhead of std::vector or std::string.
    std::unique_ptr<char[]> buffer(new char[max_utf8_len]);

    // Perform the conversion using SIMD-optimized simdutf.
    // std::u16string uses char16_t which matches simdutf's expectations.
    size_t utf8_len = simdutf::convert_utf16_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        buffer.get()
    );

    // FNV-1a 64-bit hash implementation.
    // We process the resulting UTF-8 bytes directly from the buffer.
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buffer.get());
    const uint8_t* end = ptr + utf8_len;

    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= fnv_prime;
    }

    return hash;
}
