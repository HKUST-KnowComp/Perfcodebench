#include "interface.h"
#include <string>
#include <vector>

namespace {

// Precomputed lookup table for Base64 decoding
static const signed char b64_table[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

inline uint64_t update_checksum(uint64_t hash, uint8_t byte) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    uint64_t hash = 1469598103934665603ULL;
    const size_t len = input.size();
    const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());

    for (size_t i = 0; i < len; i += 4) {
        const int c0 = b64_table[src[i]];
        const int c1 = b64_table[src[i + 1]];
        const int c2 = b64_table[src[i + 2]];
        const int c3 = b64_table[src[i + 3]];

        // Base64 decoding logic
        uint32_t triple = (static_cast<uint32_t>(c0) << 18) | 
                          (static_cast<uint32_t>(c1) << 12);
        
        hash = update_checksum(hash, static_cast<uint8_t>((triple >> 16) & 0xFF));
        
        if (c2 != -2) {
            triple |= (static_cast<uint32_t>(c2) << 6);
            hash = update_checksum(hash, static_cast<uint8_t>((triple >> 8) & 0xFF));
            
            if (c3 != -2) {
                triple |= static_cast<uint32_t>(c3);
                hash = update_checksum(hash, static_cast<uint8_t>(triple & 0xFF));
            }
        }
    }
    return hash;
}