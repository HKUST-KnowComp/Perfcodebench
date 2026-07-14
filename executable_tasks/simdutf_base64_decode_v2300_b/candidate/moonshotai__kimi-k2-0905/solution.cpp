#include "interface.h"

#include <simdutf.h>
#include <vector>
#include <cstdint>
#include <string>

namespace {

uint64_t checksum_bytes(const void* data, std::size_t len) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    thread_local std::vector<char> buffer;

    std::size_t max_out = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
    buffer.resize(max_out);

    std::size_t actual_out = 0;
    simdutf::result res = simdutf::base64_to_binary(input.data(), input.size(), buffer.data(), simdutf::base64_default, &actual_out);
    if (res != simdutf::SUCCESS) {
        return 0; // should never occur for well-formed inputs
    }

    return checksum_bytes(buffer.data(), actual_out);
}