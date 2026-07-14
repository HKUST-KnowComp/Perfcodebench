#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <vector>

namespace {

// Compute identical FNV-1a hash as in the baseline
inline uint64_t checksum_bytes(const char* data, size_t len) {
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME  = 1099511628211ULL;
    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    // Reserve binary output: ceil(src * 3/4)
    size_t bin_capacity = (input.size() + 3 / 4 * 3); // safe over-estimate
    std::vector<char> buffer(bin_capacity);

    // simdutf decode
    size_t out_len = simdutf::base64_to_binary(
        input.data(),
        input.size(),
        buffer.data(),
        simdutf::base64_url_with_padding);

    // compute checksum on the result
    return checksum_bytes(buffer.data(), out_len);
}