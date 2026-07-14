#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    const size_t max_decoded = (input.size() * 3 + 3) / 4;
    std::vector<char> out(max_decoded);

    simdutf::result res = simdutf::base64_to_binary(input.data(), input.size(), out.data());
    if (res.error) {
        return 0; // should not happen for valid base64
    }

    const size_t decoded_len = res.output_count;
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(out.data());
    for (size_t i = 0; i < decoded_len; ++i) {
        hash ^= static_cast<uint64_t>(bytes[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
