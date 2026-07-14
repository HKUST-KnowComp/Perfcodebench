#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const std::string& data) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    const char* p = data.data();
    const char* end = p + data.size();
    const char* e8 = p + (data.size() & ~size_t(7));

    while (p != e8) {
        uint64_t chunk;
        std::memcpy(&chunk, p, 8);
        for (unsigned i = 0; i < 8; ++i) {
            hash ^= ((chunk >> (i * 8)) & 0xFFU);
            hash *= prime;
        }
        p += 8;
    }
    while (p != end) {
        hash ^= static_cast<uint8_t>(*p++);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    std::string out;
    out.resize(simdutf::maximal_binary_length_from_base64(input.data(), input.size()));
    size_t written = 0;
    simdutf::result res = simdutf::base64_to_binary(input.data(), input.size(),
                                                     out.data(),
                                                     simdutf::base64_default,
                                                     &written);
    if (res != simdutf::SUCCESS) written = 0;
    out.resize(written);
    return checksum_bytes(out);
}