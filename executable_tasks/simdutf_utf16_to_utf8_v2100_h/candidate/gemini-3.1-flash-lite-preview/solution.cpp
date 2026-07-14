#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    // Max UTF-8 length for UTF-16 is 3x the number of 16-bit code units
    size_t max_len = input.size() * 3;
    std::vector<char> buffer(max_len);

    // Use simdutf to transcode efficiently
    size_t actual_len = simdutf::convert_utf16_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        buffer.data()
    );

    // FNV-1a checksum implementation
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    
    const char* data = buffer.data();
    for (size_t i = 0; i < actual_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= fnv_prime;
    }
    
    return hash;
}