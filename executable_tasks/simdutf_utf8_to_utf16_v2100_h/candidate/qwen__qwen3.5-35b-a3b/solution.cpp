#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    std::vector<char16_t> out_buffer(input.size() * 2);
    size_t out_len = simdutf::utf8_to_utf16_le(input.data(), input.size(), out_buffer.data());
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(out_buffer[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}