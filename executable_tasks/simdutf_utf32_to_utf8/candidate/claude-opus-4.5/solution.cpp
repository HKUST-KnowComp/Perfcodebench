#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    // Calculate exact UTF-8 length
    size_t utf8_len = simdutf::utf8_length_from_utf32(
        reinterpret_cast<const char32_t*>(input.data()), input.size());
    
    // Allocate buffer for UTF-8 output
    std::vector<char> utf8_buf(utf8_len);
    
    // Perform SIMD-accelerated transcoding
    simdutf::convert_utf32_to_utf8(
        reinterpret_cast<const char32_t*>(input.data()), input.size(),
        utf8_buf.data());
    
    // Compute FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(utf8_buf.data());
    const unsigned char* end = p + utf8_len;
    
    // Process 8 bytes at a time for better throughput
    while (p + 8 <= end) {
        hash ^= static_cast<uint64_t>(p[0]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[1]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[2]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[3]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[4]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[5]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[6]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(p[7]);
        hash *= 1099511628211ULL;
        p += 8;
    }
    
    // Handle remaining bytes
    while (p < end) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}