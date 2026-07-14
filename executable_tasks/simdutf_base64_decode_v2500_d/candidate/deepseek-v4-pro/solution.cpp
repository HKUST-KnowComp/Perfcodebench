#include "interface.h"
#include "simdutf.h"
#include <cstdint>
#include <memory>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    const std::size_t max_out_len = (input.size() / 4) * 3;
    auto buf = std::make_unique<char[]>(max_out_len);
    const std::size_t out_len = simdutf::base64_to_binary(
        input.data(), input.size(), buf.get());
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buf.get());
    const unsigned char* end = ptr + out_len;
    uint64_t hash = 1469598103934665603ULL;
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= 1099511628211ULL;
    }
    return hash;
}
