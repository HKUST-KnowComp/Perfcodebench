#include "interface.h"
#include <simdutf.h>
#include <memory>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    size_t out_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());
    auto buffer = std::unique_ptr<char[]>(new char[out_len]);
    simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.get());
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(buffer.get());
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
