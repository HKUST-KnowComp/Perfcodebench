#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    // simdutf guarantees that output length <= input length for UTF-16LE
    std::vector<char16_t> out(input.size(), 0);
    size_t utf16words = simdutf::convert_utf8_to_utf16le(
                            input.data(), input.size(), out.data());
    return checksum_utf16(out.data(), utf16words);
}