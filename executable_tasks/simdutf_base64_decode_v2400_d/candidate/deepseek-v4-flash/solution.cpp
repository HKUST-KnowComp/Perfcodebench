#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
    size_t max_decoded = simdutf::base64::max_decoded_length(input.size());
    std::vector<char> buffer(max_decoded);
    auto result = simdutf::base64::decode(input.data(), input.size(), buffer.data());
    // Assume valid input; result.error == simdutf::SUCCESS
    size_t decoded_length = result.count;
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < decoded_length; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}