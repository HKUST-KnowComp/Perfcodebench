#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    const size_t latin1_len = input.size();
    if (latin1_len == 0) return 1469598103934665603ULL;

    // simdutf guarantees this bound
    const size_t max_utf8_len = simdutf::utf8_length_from_latin1(input.data(), latin1_len);
    std::string out;
    out.resize(max_utf8_len);

    size_t utf8_len = simdutf::convert_latin1_to_utf8(input.data(), latin1_len, out.data());
    out.resize(utf8_len);

    return checksum_bytes(out.data(), utf8_len);
}