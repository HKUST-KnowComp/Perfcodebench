#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
    size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
    std::vector<char16_t> out(utf16_len);
    simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
    
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;
    
    const char16_t* p = out.data();
    size_t n = out.size();
    size_t i = 0;
    
    // Process 8 elements at a time for better ILP
    for (; i + 8 <= n; i += 8) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+3]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+4]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+5]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+6]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(p[i+7]);
        hash *= prime;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= prime;
    }
    
    return hash;
}