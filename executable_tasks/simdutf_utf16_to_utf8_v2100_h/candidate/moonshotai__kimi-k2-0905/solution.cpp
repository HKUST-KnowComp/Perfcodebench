#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstddef>

uint64_t transcode_checksum(const std::u16string& input) {
    const auto* src = reinterpret_cast<const char16_t*>(input.data());
    size_t len = input.size();

    simdutf::implementation impl = simdutf::get_available_implementations()["best"];
    size_t utf8_bytes = impl.utf8_length_from_utf16le(src, len);

    std::vector<char> utf8(utf8_bytes);
    size_t written = impl.convert_utf16le_to_utf8(src, len, utf8.data());
    utf8.resize(written);

    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : utf8) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}