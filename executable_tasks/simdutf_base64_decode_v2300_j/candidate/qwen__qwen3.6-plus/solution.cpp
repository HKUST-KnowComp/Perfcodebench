#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
    // Base64 decodes to at most input.size() bytes.
    std::vector<char> out(input.size());
    simdutf::result res = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    size_t len = res.count;

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
        hash *= fnv_prime;
    }
    return hash;
}
