#include "interface.h"
#include "simdutf.h"

uint64_t decode_checksum(const std::string& input) {
    std::string decoded = simdutf::base64::decode(input);
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : decoded) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}
