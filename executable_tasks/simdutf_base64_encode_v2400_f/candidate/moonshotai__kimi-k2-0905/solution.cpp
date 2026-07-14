#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

uint64_t encode_checksum(const std::string& input) {
    const size_t in_len  = input.size();
    const size_t out_len = simdutf::base64_length_from_binary(in_len);

    alignas(64) char buf[4096];          // stack buffer for small inputs
    char* out_ptr;
    bool use_heap = false;
    if (out_len > sizeof(buf)) {
        out_ptr = new char[out_len];
        use_heap = true;
    } else {
        out_ptr = buf;
    }

    // encode
    simdutf::binary_to_base64(input.data(), in_len, out_ptr);

    // compute FNV-1a checksum inline
    const uint8_t* p = reinterpret_cast<const uint8_t*>(out_ptr);
    const uint8_t* end = p + out_len;
    uint64_t hash = 1469598103934665603ULL;
    while (p + 8 <= end) {
        uint64_t v;
        std::memcpy(&v, p, 8);
        hash ^= v;
        hash *= 1099511628211ULL;
        p += 8;
    }
    while (p < end) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= 1099511628211ULL;
    }

    if (use_heap) delete[] out_ptr;
    return hash;
}