#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    std::vector<char32_t> out(input.size());
    auto res = simdutf::convert_utf8_to_utf32(input.data(), input.size(), out.data());
    out.resize(res.count);

    uint64_t hash = 1469598103934665603ULL;
    for (char32_t ch : out) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}