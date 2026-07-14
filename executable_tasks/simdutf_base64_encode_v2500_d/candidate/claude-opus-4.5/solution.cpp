#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t encode_checksum(const std::string& input) {
    size_t encoded_len = simdutf::base64_length_from_binary(input.size());
    std::string out(encoded_len, '\0');
    simdutf::binary_to_base64(input.data(), input.size(), out.data());
    
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
    const unsigned char* end = p + encoded_len;
    while (p < end) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= 1099511628211ULL;
    }
    return hash;
}