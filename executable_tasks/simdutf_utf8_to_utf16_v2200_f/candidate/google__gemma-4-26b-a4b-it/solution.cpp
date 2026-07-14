#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a style checksum for char16_t
inline uint64_t checksum_utf16(const char16_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_utf16(nullptr, 0);
    }

    // Use simdutf to transcode. 
    // We first need to determine the size or use a buffer. 
    // simdutf_utf8_to_utf16 provides highly optimized SIMD paths.
    
    // Step 1: Calculate required size for UTF-16 buffer
    // Note: simdutf doesn't provide a direct 'size_only' function in all versions, 
    // but we can use the fact that UTF-16 size is at most input.size() * 2 (for surrogates) 
    // or more accurately, we can use a temporary buffer or the library's capacity logic.
    
    // To be most efficient and avoid multiple passes, we use a pre-allocated vector.
    // A safe upper bound for UTF-16 units is input.size(). 
    // (Each UTF-8 byte results in at most 1 UTF-16 unit, except for 4-byte sequences 
    // which result in 2 units, but 4-byte UTF-8 is 4 bytes and 2 units is 4 bytes).
    // Actually, the worst case for UTF-16 units is when every character is a 4-byte UTF-8 
    // sequence (2 units) or 1-byte (1 unit). 
    // Max units = input.size() is a safe bet for most, but let's be precise.
    
    size_t max_utf16_len = input.size(); // Conservative estimate for most cases
    // For 4-byte UTF-8 (4 bytes) -> 2 UTF-16 units (4 bytes). 
    // For 1-byte UTF-8 (1 byte) -> 1 UTF-16 unit (2 bytes). 
    // The number of char16_t units is at most input.size().
    
    std::vector<char16_t> output(input.size());
    size_t utf16_len = 0;

    // simdutf_utf8_to_utf16 returns the number of char16_t written
    // It handles the SIMD acceleration internally.
    auto result = simdutf::utf8_to_utf16(input.data(), input.size(), output.data());
    
    if (result.error) {
        // This case should not be hit per task description (valid UTF-8)
        return 0;
    }

    return checksum_utf16(output.data(), result.length);
}