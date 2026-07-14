#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (char16_t ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    std::vector<char16_t> out;
    // simdutf::utf8_to_utf16le returns the number of char16_t units written
    size_t expected_len = simdutf::utf8_to_utf16le(input.data(), input.size(), nullptr);
    out.resize(expected_len);
    size_t actual_len = simdutf::utf8_to_utf16le(input.data(), input.size(), out.data());
    out.resize(actual_len);  // shrink to exact length in case of over-estimation
    return checksum_utf16(out);
}