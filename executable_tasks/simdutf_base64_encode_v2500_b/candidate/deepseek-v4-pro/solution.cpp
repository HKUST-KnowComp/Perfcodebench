#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        unsigned char ch = static_cast<unsigned char>(data[i]);
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
    const size_t out_size = ((input.size() + 2) / 3) * 4;
    constexpr size_t kStackBufferSize = 4096;
    if (out_size <= kStackBufferSize) {
        char buf[kStackBufferSize];
        char* const out = buf;
        const size_t written = simdutf::binary_to_base64(input.data(), input.size(), out);
        return checksum_bytes(out, written);
    } else {
        std::vector<char> out(out_size);
        const size_t written = simdutf::binary_to_base64(input.data(), input.size(), out.data());
        return checksum_bytes(out.data(), written);
    }
}
