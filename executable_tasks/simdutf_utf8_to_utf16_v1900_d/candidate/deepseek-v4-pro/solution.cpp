#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstddef>

uint64_t transcode_checksum(const std::string& input) {
    thread_local std::vector<char16_t> buf;
    size_t out_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
    if (buf.size() < out_len) {
        buf.resize(out_len);
    }
    size_t actual = simdutf::convert_utf8_to_utf16(input.data(), input.size(), buf.data());
    uint64_t hash = UINT64_C(1469598103934665603);
    for (size_t i = 0; i < actual; ++i) {
        hash ^= static_cast<uint64_t>(buf[i]);
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}