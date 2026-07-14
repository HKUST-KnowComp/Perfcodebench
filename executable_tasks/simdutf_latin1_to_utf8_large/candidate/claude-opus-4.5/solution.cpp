#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

uint64_t transcode_checksum(const std::string& input) {
    const size_t input_size = input.size();
    if (input_size == 0) {
        return 1469598103934665603ULL;
    }
    
    // Maximum UTF-8 length for Latin-1 is 2x input size
    const size_t max_utf8_len = input_size * 2;
    std::string out;
    out.resize(max_utf8_len);
    
    // Use simdutf for fast SIMD-accelerated conversion
    size_t utf8_len = simdutf::convert_latin1_to_utf8(
        input.data(), input_size, out.data());
    
    // Compute FNV-1a checksum with unrolling for better ILP
    const uint64_t FNV_PRIME = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
    size_t i = 0;
    
    // Process 8 bytes at a time for better instruction-level parallelism
    const size_t unroll = 8;
    const size_t limit = utf8_len - (utf8_len % unroll);
    
    for (; i < limit; i += unroll) {
        hash ^= data[i];
        hash *= FNV_PRIME;
        hash ^= data[i + 1];
        hash *= FNV_PRIME;
        hash ^= data[i + 2];
        hash *= FNV_PRIME;
        hash ^= data[i + 3];
        hash *= FNV_PRIME;
        hash ^= data[i + 4];
        hash *= FNV_PRIME;
        hash ^= data[i + 5];
        hash *= FNV_PRIME;
        hash ^= data[i + 6];
        hash *= FNV_PRIME;
        hash ^= data[i + 7];
        hash *= FNV_PRIME;
    }
    
    // Handle remaining bytes
    for (; i < utf8_len; i++) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }
    
    return hash;
}