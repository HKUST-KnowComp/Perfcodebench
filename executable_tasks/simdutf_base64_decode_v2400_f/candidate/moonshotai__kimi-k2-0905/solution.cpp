#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t update_fnv1a(uint64_t h, uint8_t b) noexcept {
    h ^= static_cast<uint64_t>(b);
    h *= 1099511628211ULL;
    return h;
}

} // anonymous namespace

uint64_t decode_checksum(const std::string& input) {
    const char* src = input.data();
    size_t src_len = input.size();

    // simdutf wants to know the expected binary length.
    // base64_to_binary will write the actual bytes consumed and produced.
    size_t max_binary_len = simdutf::maximal_binary_length_from_base64(src, src_len);
    if (max_binary_len == 0) {
        // empty or only whitespace / padding
        return 1469598103934665603ULL; // FNV-1a offset_basis
    }

    // Small on-stack buffer to avoid heap traffic for typical inputs.
    constexpr size_t STACK_BUF = 4096;
    uint8_t stack_buf[STACK_BUF];
    uint8_t* dst = stack_buf;
    bool heap_allocated = false;

    if (max_binary_len > STACK_BUF) {
        dst = new uint8_t[max_binary_len];
        heap_allocated = true;
    }

    // Perform the actual decoding.
    simdutf::result res = simdutf::base64_to_binary(src, src_len,
                                                     reinterpret_cast<char*>(dst),
                                                     simdutf::base64_default);

    // If input is invalid, simdutf returns an error; we treat it as zero bytes.
    size_t binary_len = (res.error == simdutf::error_code::SUCCESS) ? res.count : 0;

    // Compute FNV-1a checksum over the decoded bytes.
    uint64_t hash = 1469598103934665603ULL; // FNV-1a offset_basis
    for (size_t i = 0; i < binary_len; ++i) {
        hash = update_fnv1a(hash, dst[i]);
    }

    if (heap_allocated) delete[] dst;
    return hash;
}