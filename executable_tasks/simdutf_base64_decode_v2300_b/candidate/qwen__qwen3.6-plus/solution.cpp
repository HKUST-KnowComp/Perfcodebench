#include "interface.h"
#include <simdutf.h>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    std::string out;
    out.resize(input.size());
    auto result = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
    for (size_t i = 0; i < result.count; ++i) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}
