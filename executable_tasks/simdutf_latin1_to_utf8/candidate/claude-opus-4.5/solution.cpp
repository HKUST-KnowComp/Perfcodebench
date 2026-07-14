#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

uint64_t transcode_checksum(const std::string& input) {
    const char* src = input.data();
    size_t src_len = input.size();
    
    // Calculate exact UTF-8 length needed
    size_t utf8_len = simdutf::utf8_length_from_latin1(src, src_len);
    
    // Allocate output buffer
    std::string out;
    out.resize(utf8_len);
    
    // SIMD-accelerated transcoding
    simdutf::convert_latin1_to_utf8(src, src_len, out.data());
    
    // FNV-1a checksum with unrolled loop
    const uint8_t* data = reinterpret_cast<const uint8_t*>(out.data());
    size_t len = utf8_len;
    
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;
    
    // Process 8 bytes at a time
    size_t i = 0;
    size_t end8 = len & ~size_t(7);
    
    for (; i < end8; i += 8) {
        hash ^= data[i];
        hash *= prime;
        hash ^= data[i + 1];
        hash *= prime;
        hash ^= data[i + 2];
        hash *= prime;
        hash ^= data[i + 3];
        hash *= prime;
        hash ^= data[i + 4];
        hash *= prime;
        hash ^= data[i + 5];
        hash *= prime;
        hash ^= data[i + 6];
        hash *= prime;
        hash ^= data[i + 7];
        hash *= prime;
    }
    
    // Handle remaining bytes
    for (; i < len; ++i) {
        hash ^= data[i];
        hash *= prime;
    }
    
    return hash;
}
