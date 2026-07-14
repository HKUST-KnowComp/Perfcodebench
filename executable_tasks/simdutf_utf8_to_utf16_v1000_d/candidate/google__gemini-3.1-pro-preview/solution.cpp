#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

/**
 * Performance Engineer's Optimization:
 * 1. Use simdutf::utf16_length_from_utf8 to determine the exact output size.
 * 2. Use a raw buffer (std::unique_ptr) to avoid the zero-initialization overhead of std::vector::resize.
 * 3. Use simdutf::convert_utf8_to_utf16le for SIMD-accelerated transcoding.
 * 4. Compute the FNV-1a checksum on the resulting UTF-16 code units.
 */

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Determine the number of char16_t units needed.
    size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
    
    // Allocate buffer without zero-initialization.
    auto buffer = std::make_unique<char16_t[]>(utf16_len);

    // Perform SIMD-accelerated transcoding.
    // Note: The task specifies valid UTF-8, so we don't need complex error handling.
    size_t written = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), buffer.get());

    // FNV-1a hash calculation over the UTF-16 code units.
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    
    const char16_t* ptr = buffer.get();
    const char16_t* end = ptr + written;

    // Unrolling or simple loop; compilers usually optimize this well.
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= fnv_prime;
    }

    return hash;
}
