#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    const size_t dict_size = dict.size();
    std::vector<size_t> lengths(dict_size);
    std::vector<uint8_t> first_bytes(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dict[i];
        lengths[i] = s.size();
        first_bytes[i] = s.empty() ? 0 : static_cast<uint8_t>(s[0]);
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t code : codes) {
            const size_t idx = static_cast<size_t>(code);
            hash = mix(hash, static_cast<uint64_t>(lengths[idx]));
            hash = mix(hash, static_cast<uint64_t>(first_bytes[idx]));
        }
    }
    return hash;
}