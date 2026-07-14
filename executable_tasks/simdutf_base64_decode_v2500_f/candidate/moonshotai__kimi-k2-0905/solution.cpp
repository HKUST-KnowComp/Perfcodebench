#include "interface.h"
#include <simdutf.h>
#include <cstring>

namespace {

inline uint64_t fnv1a(uint64_t hash, const uint8_t* data, size_t len) {
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
    if (in_len == 0) return 1469598103934665603ULL; // FNV offset_basis

    // simdutf wants to know the exact output length in advance
    size_t out_len = 0;
    simdutf::error_code err = simdutf::base64_length_from_binary(input.data(), in_len, simdutf::base64_default, &out_len);
    if (err != simdutf::SUCCESS) {
        // malformed base64; fall back to empty checksum
        return 1469598103934665603ULL;
    }

    // reusable buffer sized to fit typical SIMD decode chunks
    alignas(64) uint8_t buffer[4096];
    uint64_t hash = 1469598103934665603ULL; // FNV offset_basis

    size_t consumed = 0;
    size_t written_total = 0;
    while (consumed < in_len) {
        size_t chunk_out = 0;
        const size_t chunk_in = std::min(in_len - consumed, sizeof(buffer) / 3 * 4);
        err = simdutf::base64_to_binary(input.data() + consumed, chunk_in,
                                         reinterpret_cast<char*>(buffer),
                                         simdutf::base64_default, &chunk_out);
        if (err != simdutf::SUCCESS) break;
        hash = fnv1a(hash, buffer, chunk_out);
        consumed += chunk_in;
        written_total += chunk_out;
    }
    return hash;
}