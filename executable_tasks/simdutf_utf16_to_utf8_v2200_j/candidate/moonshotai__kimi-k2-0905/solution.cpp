#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        unsigned char ch = static_cast<unsigned char>(data[i]);
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    const char16_t* src = reinterpret_cast<const char16_t*>(input.data());
    size_t src_len = input.size();

    size_t utf8_len = simdutf::utf8_length_from_utf16le(src, src_len);
    std::string out;
    out.resize(utf8_len);

    size_t actual_len = simdutf::convert_utf16le_to_utf8(src, src_len, out.data());
    out.resize(actual_len);

    return checksum_bytes(out.data(), out.size());
}