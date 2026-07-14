#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
    // Preallocate buffer for maximum possible decoded size
    std::vector<char> out((input.size() / 4) * 3);
    // Use simdutf for fast base64 decoding
    size_t out_len = simdutf::base64_decode(input.data(), input.size(), out.data());
    // Compute checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
