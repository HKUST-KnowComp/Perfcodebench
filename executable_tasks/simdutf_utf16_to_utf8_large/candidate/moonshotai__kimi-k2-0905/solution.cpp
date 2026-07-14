#include "interface.h"
#include <simdutf.h>
#include <string>

namespace {

uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    // simdutf expects byte length; each char16_t is 2 bytes
    const size_t utf16_bytes = input.size() * sizeof(char16_t);
    // worst-case UTF-8 length is 3 bytes per UTF-16 code unit (surrogates expand to 4)
    const size_t max_utf8_bytes = input.size() * 3;
    std::string out;
    out.resize(max_utf8_bytes);

    const auto result = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char*>(input.data()),
        utf16_bytes,
        out.data()
    );
    out.resize(result); // shrink to actual UTF-8 length
    return checksum_bytes(out);
}