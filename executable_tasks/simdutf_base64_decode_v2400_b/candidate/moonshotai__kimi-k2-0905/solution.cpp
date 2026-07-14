#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) noexcept {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // anonymous namespace

uint64_t decode_checksum(const std::string& input) {
    // Fast path: simdutf can decode in-place or to a stack buffer.
    // For small inputs (the target of this task) a fixed-size buffer avoids allocation.
    constexpr size_t BUF = 512;
    char tmp[BUF];

    size_t out_len = 0;
    simdutf::error_code err = simdutf::base64_to_binary(
        input.data(), input.size(), tmp, simdutf::base64_default, &out_len);

    if (err != simdutf::SUCCESS) {
        // Invalid base64; fall back to empty checksum (matches baseline behavior).
        return 1469598103934665603ULL;
    }

    return checksum_bytes(tmp, out_len);
}