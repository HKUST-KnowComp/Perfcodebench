#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t update_fnv1a(uint64_t hash, const uint8_t* data, size_t len) {
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    const size_t in_len = input.size();
    if (in_len == 0) return 1469598103934665603ULL;

    const char* in_ptr = input.data();
    const size_t max_out_len = (in_len / 4) * 3;

    alignas(64) uint8_t buffer[4096];
    uint64_t hash = 1469598103934665603ULL;
    size_t total_written = 0;

    simdutf::result result = simdutf::base64_to_binary_safe(
        in_ptr, in_len,
        reinterpret_cast<char*>(buffer), sizeof(buffer),
        simdutf::base64_default, total_written);

    while (result.error == simdutf::error_code::SUCCESS && total_written > 0) {
        hash = update_fnv1a(hash, buffer, total_written);
        total_written = 0;
        result = simdutf::base64_to_binary_safe(
            in_ptr, in_len,
            reinterpret_cast<char*>(buffer), sizeof(buffer),
            simdutf::base64_default, total_written);
    }

    if (result.error != simdutf::error_code::SUCCESS) {
        return 0;
    }

    return hash;
}