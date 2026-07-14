#pragma once

#include <cstdint>
#include <string>
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input) {
    auto result = simdutf::base64_decode(input);
    if (result.error) {
        return 0;
    }
    const char* decoded = result.value;
    size_t decoded_size = result.size;

    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < decoded_size; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(decoded[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}