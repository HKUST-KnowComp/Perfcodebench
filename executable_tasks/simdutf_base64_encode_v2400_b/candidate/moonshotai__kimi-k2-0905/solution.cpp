#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstring>

namespace {

uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
    const std::size_t len = input.size();
    const std::size_t out_len = ((len + 2) / 3) * 4;
    std::string out;
    out.resize(out_len);

    // Fast SIMD path for the bulk
    std::size_t written = simdutf::binary_to_base64(
        input.data(),
        len,
        out.data(),
        simdutf::base64_default
    );
    out.resize(written); // simdutf returns exact length

    return checksum_bytes(out);
}