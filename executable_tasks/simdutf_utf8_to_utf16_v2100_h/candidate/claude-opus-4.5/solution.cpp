#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    // Use simdutf for fast UTF-8 to UTF-16 transcoding
    size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
    
    std::vector<char16_t> out(utf16_len);
    
    simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
    
    // Compute FNV-1a checksum with unrolling
    uint64_t hash = 1469598103934665603ULL;
    const char16_t* data = out.data();
    size_t n = out.size();
    
    // Process 4 elements at a time
    size_t i = 0;
    size_t limit = n & ~size_t(3);
    for (; i < limit; i += 4) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 1]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 2]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 3]);
        hash *= 1099511628211ULL;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}