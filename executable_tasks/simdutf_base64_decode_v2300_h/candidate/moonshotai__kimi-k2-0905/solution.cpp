#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    // simdutf wants non-const pointer; safe because it only reads
    char* src = const_cast<char*>(input.data());
    size_t src_len = input.size();

    // worst-case decoded size
    std::vector<char> buf((src_len / 4) * 3 + 1);

    // standard base64 with optional padding
    simdutf::result r = simdutf::base64_to_binary(src, src_len,
                                                   buf.data(),
                                                   simdutf::base64_default);
    if (r.error) {
        // on invalid input return same checksum as baseline would for empty string
        return 1469598103934665603ULL;
    }
    return checksum_bytes(buf.data(), r.count);
}