#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    const char16_t* src = input.data();
    size_t src_len = input.size();
    
    // Get exact UTF-8 length
    size_t utf8_len = simdutf::utf8_length_from_utf16le(src, src_len);
    
    // Allocate buffer
    char* utf8_buf = new char[utf8_len];
    
    // Transcode using SIMD
    simdutf::convert_utf16le_to_utf8(src, src_len, utf8_buf);
    
    // Compute FNV-1a checksum with unrolling
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(utf8_buf);
    size_t i = 0;
    size_t len8 = utf8_len & ~7ULL;
    
    for (; i < len8; i += 8) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 3]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 4]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 5]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 6]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 7]);
        hash *= prime;
    }
    
    for (; i < utf8_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    
    delete[] utf8_buf;
    
    return hash;
}