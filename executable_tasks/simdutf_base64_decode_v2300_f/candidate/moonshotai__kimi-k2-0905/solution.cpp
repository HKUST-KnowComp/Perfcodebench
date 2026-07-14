#include "interface.h"

#include <simdutf.h>

namespace {

uint64_t checksum_bytes(std::string_view data) noexcept {
    uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : data) {
        h ^= static_cast<uint64_t>(c);
        h *= 1099511628211ULL;
    }
    return h;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    size_t max_binary_len = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
    std::string buffer;
    buffer.resize(max_binary_len);

    size_t actual_len = simdutf::base64_to_binary(input.data(), input.size(),
                                                  buffer.data(),
                                                  simdutf::base64_default);
    buffer.resize(actual_len);
    return checksum_bytes(buffer);
}