#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

/**
 * Performance Engineer's Optimization:
 * 1. Use simdutf::utf32_length_from_utf8 to determine the exact output size.
 * 2. Use simdutf::convert_utf8_to_utf32 for SIMD-accelerated transcoding.
 * 3. Use a stack-allocated buffer for small inputs to avoid heap allocation overhead.
 * 4. Calculate the FNV-1a checksum in a tight loop over the resulting UTF-32 buffer.
 */

uint64_t transcode_checksum(const std::string& input) {
    const size_t utf8_len = input.size();
    if (utf8_len == 0) {
        return 1469598103934665603ULL;
    }

    // Determine the number of UTF-32 code points
    const size_t utf32_len = simdutf::utf32_length_from_utf8(input.data(), utf8_len);

    // Use a stack buffer for small strings to avoid allocation latency
    constexpr size_t STACK_LIMIT = 2048;
    char32_t stack_buffer[STACK_LIMIT];
    std::unique_ptr<char32_t[]> heap_buffer;
    char32_t* target_ptr;

    if (utf32_len <= STACK_LIMIT) {
        target_ptr = stack_buffer;
    } else {
        heap_buffer = std::make_unique<char32_t[]>(utf32_len);
        target_ptr = heap_buffer.get();
    }

    // Perform the SIMD-accelerated conversion
    // Note: The task assumes valid UTF-8 as per the baseline logic.
    simdutf::convert_utf8_to_utf32(input.data(), utf8_len, target_ptr);

    // FNV-1a 64-bit hash calculation
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < utf32_len; ++i) {
        hash ^= static_cast<uint64_t>(target_ptr[i]);
        hash *= fnv_prime;
    }

    return hash;
}