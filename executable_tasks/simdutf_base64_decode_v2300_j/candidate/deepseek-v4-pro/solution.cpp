#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
    // Maximum possible decoded size: 3 bytes for each 4 base64 characters.
    // simdutf will write exactly the correct number of bytes (respecting padding).
    const size_t max_out_len = (input.size() / 4) * 3;
    std::vector<char> decoded(max_out_len);
    
    // Use simdutf's highly-optimized base64 decoder.
    simdutf::result r = simdutf::base64_to_binary(
        input.data(), input.size(), decoded.data());
    // Input is guaranteed to be valid base64, so we trust r.error == 0.
    const size_t out_len = r.count;
    
    // Compute the same checksum as the baseline.
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(decoded.data());
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}