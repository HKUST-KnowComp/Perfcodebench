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
    static constexpr size_t BUF_SIZE = 65536 / sizeof(char16_t);
    std::vector<char16_t> buffer(BUF_SIZE);
    simdutf::utf8_to_utf16le_state state{};
    const char* src = input.data();
    size_t src_left = input.size();

    uint64_t hash = 1469598103934665603ULL;

    while (src_left > 0) {
        char16_t* dst = buffer.data();
        size_t dst_left = buffer.size();
        auto result = simdutf::convert_utf8_to_utf16le_with_state(
            state, src, src_left, dst, dst_left);
        size_t produced = buffer.size() - dst_left;
        hash = checksum_utf16(buffer.data(), produced);
    }
    return hash;
}