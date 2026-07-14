#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
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

    // simdutf::utf16_to_utf8 requires a destination buffer.
    // The maximum size for UTF-8 is 3 bytes per UTF-16 unit (for BMP) 
    // or 4 bytes for surrogate pairs (which use 2 UTF-16 units).
    // A safe upper bound is input.size() * 3.
    std::vector<char> buffer(input.size() * 3);
    
    // simdutf::utf16_to_utf8 returns the number of bytes written to the destination.
    // It handles the UTF-16LE to UTF-8 conversion using SIMD if available.
    auto result = simdutf::utf16_to_utf8(
        reinterpret_cast<const char*>(input.data()), 
        input.size() * 2, 
        buffer.data(), 
        buffer.size()
    );

    if (result.error) {
        // In case of error (invalid UTF-16), the task implies valid input,
        // but we handle it by returning the checksum of the partially converted string.
        return checksum_bytes(buffer.data(), result.bytes_written);
    }

    return checksum_bytes(buffer.data(), result.bytes_written);
}