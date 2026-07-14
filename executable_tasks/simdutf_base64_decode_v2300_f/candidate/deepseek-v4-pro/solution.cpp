#include "interface.h"
#include <simdutf.h>
#include <memory>
#include <cstdint>

uint64_t decode_checksum(const std::string& input) {
    const size_t max_out = (input.size() / 4) * 3;
    std::unique_ptr<char[]> buffer(new char[max_out]);
    size_t actual = simdutf::base64_decode(input.data(), input.size(), buffer.get());
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < actual; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}