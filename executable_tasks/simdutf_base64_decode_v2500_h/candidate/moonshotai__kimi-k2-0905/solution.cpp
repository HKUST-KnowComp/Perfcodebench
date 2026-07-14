#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>

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
    // simdutf wants a writable output buffer; worst case is (input.size()/4)*3.
    std::vector<char> buf((input.size() / 4) * 3 + 3);
    const auto result = simdutf::base64_to_binary(input.data(), input.size(),
                                                   buf.data(),
                                                   simdutf::base64_default);
    // result.count is the number of decoded bytes; result.error is always success here.
    return checksum_bytes(buf.data(), result.count);
}