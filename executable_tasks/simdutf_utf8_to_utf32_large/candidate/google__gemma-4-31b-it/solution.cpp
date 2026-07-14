#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Use simdutf to determine the required size for the output buffer
    size_t utf32_len = simdutf::utf8_to_utf32_size(input.data(), input.size());
    
    // Allocate a buffer on the stack for small inputs or heap for large ones
    // To avoid repeated allocations in a performance loop, we use a vector
    // but we can optimize by using a fixed-size buffer if the input size is bounded.
    std::vector<char32_t> out(utf32_len);

    // Perform the high-performance SIMD transcoding
    simdutf::utf8_to_utf32(input.data(), input.size(), out.data());

    // Compute the FNV-1a style checksum over the resulting UTF-32 sequence
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf32_len; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}