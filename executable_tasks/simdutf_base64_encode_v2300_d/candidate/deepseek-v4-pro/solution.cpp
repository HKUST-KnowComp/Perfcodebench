#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>
#include <string>

namespace {
    constexpr uint64_t kHashInit = 1469598103934665603ULL;
    constexpr uint64_t kHashMul = 1099511628211ULL;

    uint64_t checksum_bytes(const char* data, size_t len) {
        uint64_t hash = kHashInit;
        for (size_t i = 0; i < len; ++i) {
            unsigned char ch = static_cast<unsigned char>(data[i]);
            hash ^= static_cast<uint64_t>(ch);
            hash *= kHashMul;
        }
        return hash;
    }
}

uint64_t encode_checksum(const std::string& input) {
    size_t input_len = input.size();
    size_t output_len = ((input_len + 2) / 3) * 4;
    std::vector<char> out(output_len);
    size_t written = simdutf::binary_to_base64(input.data(), input_len, out.data());
    return checksum_bytes(out.data(), written);
}