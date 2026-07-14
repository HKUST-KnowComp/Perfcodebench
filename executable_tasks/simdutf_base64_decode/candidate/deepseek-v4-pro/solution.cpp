#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t decode_checksum(const std::string& input) {
    std::size_t out_capacity = ((input.size() + 3) / 4) * 3;
    std::string out(out_capacity, '\0');
    auto result = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    out.resize(result.count);
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}