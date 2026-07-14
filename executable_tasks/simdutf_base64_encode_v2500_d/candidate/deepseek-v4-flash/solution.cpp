#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>

uint64_t encode_checksum(const std::string& input) {
    std::string out = simdutf::base64::encode(input.data(), input.size());
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
