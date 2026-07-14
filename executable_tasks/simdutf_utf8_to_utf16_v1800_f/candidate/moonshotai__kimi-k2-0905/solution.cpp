#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

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
    // simdutf guarantees that UTF-16 length never exceeds UTF-8 length
    const size_t utf8_len = input.size();
    std::vector<char16_t> buffer(utf8_len + 1);  // +1 for over-estimate safety

    // convert to UTF-16LE in host byte order (simdutf produces native endianness)
    size_t utf16_len = simdutf::convert_utf8_to_utf16(input.data(), utf8_len,
                                                      buffer.data());

    return checksum_utf16(buffer.data(), utf16_len);
}