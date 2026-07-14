#include "interface.h"
#include "simdutf.h"
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
    const size_t utf16_len = input.size();
    if (utf16_len == 0) return 1469598103934665603ULL;

    const simdutf::encoding_type utf16_type = simdutf::encoding_type::UTF16_LE;
    const size_t utf8_max_len = simdutf::utf8_length_from_utf16(input.data(), utf16_len, utf16_type);
    std::string utf8_out;
    utf8_out.resize(utf8_max_len);

    const size_t utf8_actual_len = simdutf::convert_utf16_to_utf8(input.data(), utf16_len,
                                                                    utf8_out.data(), utf16_type);
    utf8_out.resize(utf8_actual_len);

    return checksum_bytes(utf8_out.data(), utf8_actual_len);
}