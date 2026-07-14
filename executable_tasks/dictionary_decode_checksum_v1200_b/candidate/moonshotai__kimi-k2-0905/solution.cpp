#include "interface.h"
#include <cstdint>
#include <vector>
#include <string>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters)
{
    const std::size_t n = codes.size();
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = hash;
        for (std::size_t i = 0; i < n; ++i) {
            const std::string& tok = dict[static_cast<std::size_t>(codes[i])];
            h = mix(h, tok.size());
            h = mix(h, tok.empty() ? 0u : static_cast<unsigned char>(tok[0]));
        }
        hash = h;
    }
    return hash;
}