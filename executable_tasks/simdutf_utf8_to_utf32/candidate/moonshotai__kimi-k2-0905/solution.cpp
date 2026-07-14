#include "interface.h"
#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_utf32(const char32_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    const size_t max_utf32_len = simdutf::utf32_length_from_utf8(input.data(), input.size());
    std::vector<char32_t> out(max_utf32_len);
    const size_t utf32_len = simdutf::convert_utf8_to_utf32(input.data(), input.size(), out.data());
    return checksum_utf32(out.data(), utf32_len);
}