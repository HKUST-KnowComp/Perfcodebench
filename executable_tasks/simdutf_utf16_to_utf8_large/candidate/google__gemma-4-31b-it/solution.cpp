#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* udata = reinterpret_cast<const unsigned char*>(data);
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(udata[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // UTF-8 can take up to 3 bytes for a BMP character and 4 bytes for a surrogate pair (2 UTF-16 units).
    // Max expansion is 3 bytes per UTF-16 unit (e.g., 0x0800 -> 3 bytes).
    size_t max_utf8_size = input.size() * 3;
    std::string buffer;
    buffer.resize(max_utf8_size);

    // simdutf::utf16_to_utf8 returns the number of bytes written to the destination.
    // It is highly optimized using SIMD instructions.
    auto result = simdutf::utf16_to_utf8(input.data(), input.size(), buffer.data(), max_utf8_size);
    
    if (result.error) {
        // The task specifies valid UTF-16LE input, so this path is theoretically unreachable.
        return 0;
    }

    return checksum_bytes(buffer.data(), result.bytes_written);
}