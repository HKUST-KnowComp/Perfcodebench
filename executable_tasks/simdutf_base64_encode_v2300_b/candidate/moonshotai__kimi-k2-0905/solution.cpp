#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstring>

uint64_t encode_checksum(const std::string& input) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;

    uint64_t hash = fnv_offset;

    if (input.empty()) return hash;

    /* worst-case base64 length */
    const size_t max_out = simdutf::base64_length_from_binary(input.size());
    char* buf = static_cast<char*>(std::malloc(max_out));
    if (!buf) return 0; /* malloc failed – return dummy hash */

    size_t written = simdutf::base64_encode(input.data(), input.size(), buf);
    /* written is always exactly max_out for standard base64 */

    /* fused checksum */
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[i]));
        hash *= fnv_prime;
    }

    std::free(buf);
    return hash;
}