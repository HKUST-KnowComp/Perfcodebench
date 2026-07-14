#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    // Allocate enough space for the maximum possible output
    size_t max_out = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
    std::vector<char> out(max_out);
    // Perform SIMD-accelerated base64 decoding
    size_t actual = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    // Compute checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < actual; ++i) {
        unsigned char ch = static_cast<unsigned char>(out[i]);
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
