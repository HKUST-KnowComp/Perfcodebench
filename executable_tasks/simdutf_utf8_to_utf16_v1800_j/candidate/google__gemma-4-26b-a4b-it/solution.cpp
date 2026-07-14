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

    // Pre-allocate buffer. UTF-16 size is at most input.size() * 2 (for surrogates) 
    // but usually input.size() for ASCII. simdutf handles the sizing/allocation logic 
    // efficiently if we provide a buffer or use its internal allocation.
    // To minimize overhead, we use a vector and resize it.
    
    // We use simdutf's conversion which is significantly faster than scalar loops.
    // Since we need to return a checksum over the exact sequence, we must ensure 
    // the conversion is correct. simdutf::convert_utf8_to_utf16 is highly optimized.
    
    size_t max_utf16_len = input.size(); // Heuristic, but we need to handle surrogates.
    // A safe upper bound for UTF-16 code units is input.size() * 2 (worst case for 1-byte UTF8 to 2-byte UTF16)
    // However, for valid UTF-8, the number of UTF-16 units is at most input.size().
    // Actually, 1-byte UTF8 -> 1 UTF16, 2-byte UTF8 -> 1 UTF16, 3-byte UTF8 -> 1 UTF16, 4-byte UTF8 -> 2 UTF16.
    // The maximum number of 16-bit units is indeed input.size() (e.g., all 1-byte chars).
    // Wait, if input is 4-byte UTF8 (1 char), it produces 2 UTF16 units. 
    // So max units = input.size() * 2 is a safe upper bound.
    
    std::vector<char16_t> out(input.size() * 2);
    size_t utf16_len = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());
    
    return checksum_utf16(out.data(), utf16_len);
}