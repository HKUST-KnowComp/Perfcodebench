#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>
#include <cstddef>

uint64_t decode_checksum(const std::string& input) {
    // Maximum possible decoded length for base64
    size_t max_len = simdutf::base64::decoded_length(input.size());
    std::vector<char> buf(max_len);
    // Decode base64 using SIMD
    auto res = simdutf::base64::decode(input.data(), input.size(), buf.data());
    // Assume input is valid; if error occurs, checksum result may not match baseline.
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < res.count; ++i) {
        unsigned char ch = static_cast<unsigned char>(buf[i]);
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}