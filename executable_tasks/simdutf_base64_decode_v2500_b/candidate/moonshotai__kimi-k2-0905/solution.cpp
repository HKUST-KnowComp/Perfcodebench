#include "interface.h"
#include <simdutf.h>
#include <cstdint>

extern "C" uint64_t decode_checksum(const std::string& input) {
    // Fast length check without the need to decode the entire body
    const size_t inputLen = input.size();
    size_t outLen = 0;
    bool success = simdutf::base64_to_binary(
                       input.data(),
                       inputLen,
                       nullptr, // NULL destination triggers fast path
                       simdutf::decode_base64_options_input_is_binary,
                       simdutf::base64_default,
                       &outLen);
    (void)(success);

    // Pre-allocate exact destination size
    uint8_t* dst = new uint8_t[outLen];

    // Actual SIMD‐accelerated base64 decode
    size_t written = simdutf::base64_to_binary(
                         reinterpret_cast<const char*>(input.data()),
                         inputLen,
                         reinterpret_cast<char*>(dst),
                         simdutf::decode_base64_options_input_is_binary,
                         simdutf::base64_default);

    // Rolling 64-bit FNV-1a checksum
    uint64_t hash = 14695981039346656037ULL;
    const uint8_t* end = dst + written;
    for (uint8_t* p = dst; p != end; ++p) {
        hash ^= *p;
        hash *= 1099511628211ULL;
    }

    delete[] dst;
    return hash;
}