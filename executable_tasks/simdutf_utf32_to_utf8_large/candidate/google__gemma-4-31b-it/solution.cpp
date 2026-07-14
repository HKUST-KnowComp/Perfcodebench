#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Pre-calculate the required size for the UTF-8 buffer to avoid reallocations.
    // simdutf::convert_utf32_to_utf8 can provide the exact size or we can use a safe upper bound.
    // For performance in large workloads, we use a buffer that can hold the worst-case size.
    std::vector<uint8_t> buffer(input.size() * 4);
    
    // Use simdutf for high-performance SIMD-accelerated transcoding
    auto result = simdutf::convert_utf32_to_utf8(input.data(), input.size(), buffer.data(), buffer.size());
    
    // The result contains the number of bytes written to the buffer
    size_t bytes_written = result.bytes_written;
    const uint8_t* data = buffer.data();

    // Optimized FNV-1a 64-bit checksum
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < bytes_written; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}