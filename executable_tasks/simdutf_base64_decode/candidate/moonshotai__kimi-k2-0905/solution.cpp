#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) noexcept {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint8_t>(data[i]);
        h *= prime;
    }
    return h;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    static const simdutf::implementation* impl = simdutf::get_available_implementations().best();
    const size_t max_binary_len = impl->max_binary_length_from_base64(input.data(), input.size());
    std::vector<char> buffer(max_binary_len);

    const size_t actual_len = impl->base64_to_binary(
        input.data(), input.size(), buffer.data(), simdutf::base64_default);

    return checksum_bytes(buffer.data(), actual_len);
}