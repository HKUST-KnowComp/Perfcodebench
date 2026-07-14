#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    std::vector<char> out((input.size() * 3) / 4 + 4);
    auto result = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    size_t len = result.count;
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= fnv_prime;
    }
    
    return hash;
}