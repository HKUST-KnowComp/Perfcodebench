#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>
#include <cstddef>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    // Calculate required UTF-8 buffer size
    size_t utf8_len = simdutf::utf8_length_from_utf16le(
        reinterpret_cast<const char16_t*>(input.data()), input.size());
    
    // Allocate buffer and perform conversion
    std::string out(utf8_len, '\0');
    simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), input.size(),
        out.data());
    
    // FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
    size_t len = out.size();
    
    // Process 8 bytes at a time for better throughput
    size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 1]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 2]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 3]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 4]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 5]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 6]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(data[i + 7]);
        hash *= 1099511628211ULL;
    }
    
    // Handle remaining bytes
    for (; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}