#include "interface.h"
#include "simdutf.h"
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

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    const size_t max_utf8_bytes = input.size() * 3;
    std::vector<char> buffer(max_utf8_bytes);

    const auto result = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()),
        input.size(),
        buffer.data()
    );

    return checksum_bytes(buffer.data(), result);
}