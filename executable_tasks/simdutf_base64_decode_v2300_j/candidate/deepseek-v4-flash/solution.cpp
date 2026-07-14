#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input) {
    // Determine maximum decoded length
    size_t max_len = simdutf::base64::decoded_length(input.size());
    std::vector<char> buf(max_len);
    // Decode using SIMD-optimized routine
    size_t decoded_len = simdutf::base64::decode(input.data(), input.size(), buf.data());
    // Compute FNV-1a hash over decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const char* data = buf.data();
    for (size_t i = 0; i < decoded_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}