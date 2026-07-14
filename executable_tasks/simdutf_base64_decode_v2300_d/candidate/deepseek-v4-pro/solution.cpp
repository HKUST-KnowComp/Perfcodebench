#include "interface.h"
#include "simdutf.h"
#include <memory>
#include <cstddef>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t kMul = 1099511628211ULL;
    std::size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        auto ch0 = static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        auto ch1 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+1]));
        auto ch2 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+2]));
        auto ch3 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+3]));
        auto ch4 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+4]));
        auto ch5 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+5]));
        auto ch6 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+6]));
        auto ch7 = static_cast<uint64_t>(static_cast<unsigned char>(data[i+7]));
        hash ^= ch0; hash *= kMul;
        hash ^= ch1; hash *= kMul;
        hash ^= ch2; hash *= kMul;
        hash ^= ch3; hash *= kMul;
        hash ^= ch4; hash *= kMul;
        hash ^= ch5; hash *= kMul;
        hash ^= ch6; hash *= kMul;
        hash ^= ch7; hash *= kMul;
    }
    for (; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= kMul;
    }
    return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
    const std::size_t max_out = simdutf::base64_decoded_length(input.size());
    auto buf = std::make_unique<char[]>(max_out);
    const std::size_t actual = simdutf::base64_decode(input.data(), input.size(), buf.get());
    return checksum_bytes(buf.get(), actual);
}
