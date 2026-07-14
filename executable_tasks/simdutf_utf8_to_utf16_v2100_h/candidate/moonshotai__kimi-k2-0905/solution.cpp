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

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    const size_t max_utf16 = input.size();          // worst case: 1 byte -> 1 UTF-16 code unit
    std::vector<char16_t> out(max_utf16);

    size_t utf16_words = simdutf::convert_utf8_to_utf16le(
                            input.data(),
                            input.size(),
                            reinterpret_cast<char16_t*>(out.data()));

    return checksum_utf16(out.data(), utf16_words);
}