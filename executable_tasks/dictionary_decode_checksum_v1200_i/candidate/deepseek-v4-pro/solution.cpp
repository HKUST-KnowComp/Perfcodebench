#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

struct DictInfo {
    std::size_t size;
    unsigned char first;
};

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
    if (iters <= 0) {
        return 0;  // original returns initial 0 when iters==0
    }

    // Precompute sizes and first bytes for all dictionary entries to avoid
    // repeated string accesses during the iteration.
    std::vector<DictInfo> info;
    info.reserve(dict.size());
    for (const auto& token : dict) {
        DictInfo d;
        d.size = token.size();
        d.first = token.empty() ? 0 : static_cast<unsigned char>(token[0]);
        info.push_back(d);
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t code : codes) {
            const auto& d = info[static_cast<std::size_t>(code)];
            hash = mix(hash, d.size);
            hash = mix(hash, d.first);
        }
    }
    return hash;
}