#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // simdutf expects char32_t input as uint32_t*
    const uint32_t* data = reinterpret_cast<const uint32_t*>(input.data());
    size_t len = input.size();

    // Calculate required buffer size
    size_t max_len = simdutf::utf8_length_from_utf32(data, len);
    std::vector<uint8_t> buffer(max_len);

    // Transcode using simdutf
    size_t actual_len = simdutf::convert_utf32_to_utf8(data, len, reinterpret_cast<char*>(buffer.data()));

    // FNV-1a checksum calculation
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < actual_len; ++i) {
        hash ^= static_cast<uint64_t>(buffer[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}