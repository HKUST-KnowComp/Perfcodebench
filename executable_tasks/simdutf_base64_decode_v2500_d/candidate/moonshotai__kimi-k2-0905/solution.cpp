#include "interface.h"
#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    static thread_local std::vector<char> buf;
    buf.clear();

    size_t max_binary_len = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
    buf.resize(max_binary_len);

    simdutf::result res = simdutf::base64_to_binary(input.data(), input.size(),
                                                     buf.data(),
                                                     simdutf::base64_default);
    if (res.error) return 0; // should not happen for valid inputs per task contract

    return checksum_bytes(buf.data(), res.count);
}