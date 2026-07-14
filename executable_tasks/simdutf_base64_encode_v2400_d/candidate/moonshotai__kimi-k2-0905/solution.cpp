#include "interface.h"
#include <simdutf.h>
#include <immintrin.h>
#include <cstddef>
#include <cstdint>
#include <string>

namespace {

// FNV-1a 64-bit hash computed incrementally
inline uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t h = 14695981039346656037ULL; // FNV offset_basis
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint8_t>(data[i]);
        h *= 1099511628211ULL; // FNV prime
    }
    return h;
}

// Compute base64 output length exactly as simdutf encodes it (no padding)
inline size_t base64_encoded_length(size_t binary_len) {
    return (binary_len / 3) * 3 + ((binary_len % 3) * 8 + 4) / 6;
}

} // anonymous namespace

uint64_t encode_checksum(const std::string& input) {
    if (input.empty()) return 14695981039346656037ULL;

    const size_t max_out = ((input.size() + 2) / 3) * 4);
    alignas(64) char buf[4096]; // hot stack buffer for typical sizes
    char* dst = nullptr;
    bool using_heap = false;

    if (max_out <= sizeof(buf)) {
        dst = buf;
    } else {
        dst = new char[max_out];
        using_heap = true;
    }

    // Perform base64 encoding without padding (simdutf default)
    const size_t enc_len = simdutf::binary_to_base64(
        input.data(), input.size(), dst);

    // Compute checksum over the encoded bytes
    const uint64_t cs = checksum_bytes(dst, enc_len);

    if (using_heap) delete[] dst;

    return cs;
}