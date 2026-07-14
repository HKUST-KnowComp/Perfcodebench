#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    size_t len = input.size();
    size_t out_len = simdutf::utf16_length_from_utf8(input.data(), len);
    std::vector<char16_t> out(out_len);
    simdutf::utf8_to_utf16le(input.data(), len, out.data());

    uint64_t hash = 1469598103934665603ULL;
    const char16_t* ptr = out.data();
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
