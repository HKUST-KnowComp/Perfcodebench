#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) return 1469598103934665603ULL;

    const size_t max_utf8_bytes = input.size() * 4;
    std::vector<char> buffer;
    buffer.resize(max_utf8_bytes);

    size_t utf8_len = simdutf::convert_utf32_to_utf8(
        reinterpret_cast<const char32_t*>(input.data()),
        input.size(),
        buffer.data()
    );

    return checksum_bytes(buffer.data(), utf8_len);
}