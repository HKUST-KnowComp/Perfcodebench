#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t decode_checksum(const std::string& input) {
    size_t max_len = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
    std::vector<char> out(max_len);
    simdutf::result r = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    size_t decoded_len = r.count;
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(out.data());
    for (size_t i = 0; i < decoded_len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
