#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

/**
 * Performance Engineer's Optimization Notes:
 * 1. Used simdutf::convert_utf8_to_utf16le for SIMD-accelerated transcoding.
 * 2. Avoided the extra pass of simdutf::utf16_length_from_utf8 because 
 *    input.size() is a guaranteed upper bound for the number of char16_t units.
 *    (1-3 byte UTF-8 -> 1 char16_t; 4 byte UTF-8 -> 2 char16_t).
 * 3. Implemented a small-buffer optimization (SBO) using a stack array for 
 *    strings up to 2048 bytes to eliminate heap allocation latency for small inputs.
 * 4. Calculated the FNV-1a checksum directly on the transcoded buffer.
 */

uint64_t transcode_checksum(const std::string& input) {
    const size_t n = input.size();
    if (n == 0) {
        return 1469598103934665603ULL;
    }

    // Use stack for small strings, heap for large ones.
    // input.size() is always >= required char16_t count.
    char16_t stack_buffer[1024];
    std::unique_ptr<char16_t[]> heap_buffer;
    char16_t* buf_ptr = stack_buffer;

    if (n > 1024) {
        heap_buffer.reset(new char16_t[n]);
        buf_ptr = heap_buffer.get();
    }

    // Perform SIMD transcoding
    size_t utf16_len = simdutf::convert_utf8_to_utf16le(input.data(), n, buf_ptr);

    // FNV-1a checksum calculation
    uint64_t hash = 1469598103934665603ULL;
    const char16_t* const end = buf_ptr + utf16_len;
    
    // Unrolling or simple loop; compilers usually optimize this well.
    for (const char16_t* p = buf_ptr; p < end; ++p) {
        hash ^= static_cast<uint64_t>(*p);
        hash *= 1099511628211ULL;
    }

    return hash;
}