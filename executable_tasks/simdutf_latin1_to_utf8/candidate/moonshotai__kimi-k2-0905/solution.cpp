#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
    // simdutf guarantees that the output will never be larger than 2*input.size()
    std::string out;
    out.resize(simdutf::latin1_to_utf8_max_length(input.size()));
    size_t utf8_len = simdutf::convert_latin1_to_utf8(input.data(), input.size(), out.data());
    out.resize(utf8_len);
    return checksum_bytes(out);
}